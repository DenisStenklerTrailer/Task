#include "homing.h"
#include <stddef.h>


void Homing_Init(Homing *h, const HomingConfig *config)
{
    if (h == NULL)
    {
        return;
    }

    h->state = HOMING_IDLE;
    h->error = HOMING_ERR_NONE;
    h->command = ACTUATOR_STOP;

    h->state_start_ms = 0U;
    h->travel_start_ms = 0U;

    h->extend_time_ms = 0U;
    h->retract_time_ms = 0U;

    h->config.seek_timeout_ms = 0U;
    h->config.min_travel_ms = 0U;
    h->config.max_travel_ms = 0U;

    if (config == NULL)
    {
        h->state = HOMING_ERROR;
        h->error = HOMING_ERR_INVALID_CONFIG;
        return;
    }

    h->config = *config;

    if (config->seek_timeout_ms == 0U ||
        config->min_travel_ms == 0U ||
        config->min_travel_ms > config->max_travel_ms)
    {
        h->state = HOMING_ERROR;
        h->error = HOMING_ERR_INVALID_CONFIG;
        return;
    }
}




bool Homing_Start(Homing *h, uint32_t now_ms)
{
    if (h == NULL)
    {
        return false;
    }

    /* Validate configuration before allowing a restart. */
    if (h->config.seek_timeout_ms == 0U ||
        h->config.min_travel_ms == 0U ||
        h->config.min_travel_ms > h->config.max_travel_ms)
    {
        h->state = HOMING_ERROR;
        h->error = HOMING_ERR_INVALID_CONFIG;
        h->command = ACTUATOR_STOP;
        return false;
    }

    if (h->state != HOMING_IDLE &&
        h->state != HOMING_DONE &&
        h->state != HOMING_ERROR)
    {
        return false;
    }

    h->state = HOMING_SEEK_LEFT;
    h->error = HOMING_ERR_NONE;
    h->command = ACTUATOR_RETRACT;

    h->state_start_ms = now_ms;
    h->travel_start_ms = now_ms;

    h->extend_time_ms = 0U;
    h->retract_time_ms = 0U;

    return true;
}



static void Homing_SetError(Homing *h, HomingError error)
{
    h->error = error;
    h->state = HOMING_ERROR;
    h->command = ACTUATOR_STOP;
}


static bool Homing_Elapsed(uint32_t now_ms,
                           uint32_t start_ms,
                           uint32_t timeout_ms)
{
    return (uint32_t)(now_ms - start_ms) >= timeout_ms;
}




void Homing_Update(Homing *h, uint32_t now_ms, HomingInputs inputs)
{
    if (h == NULL)
    {
        return;
    }

    if (inputs.left_switch && inputs.right_switch)
    {
        Homing_SetError(h, HOMING_ERR_BOTH_SWITCHES);
        return;
    }

    switch (h->state)
    {
        case HOMING_IDLE:
        case HOMING_DONE:
        case HOMING_ERROR:
            h->command = ACTUATOR_STOP;
            break;

        case HOMING_SEEK_LEFT:
            if (Homing_Elapsed(now_ms, h->state_start_ms,
                               h->config.seek_timeout_ms))
            {
                Homing_SetError(h, HOMING_ERR_TIMEOUT);
            }
            else if (inputs.left_switch)
            {
                h->command = ACTUATOR_STOP;
                h->state = HOMING_SEEK_RIGHT;
                h->state_start_ms = now_ms;
                h->travel_start_ms = now_ms;
            }
            else
            {
                h->command = ACTUATOR_RETRACT;
            }
            break;

        case HOMING_SEEK_RIGHT:
            if (Homing_Elapsed(now_ms, h->state_start_ms,
                               h->config.seek_timeout_ms))
            {
                Homing_SetError(h, HOMING_ERR_TIMEOUT);
            }
            else if (inputs.right_switch)
            {
                h->extend_time_ms = now_ms - h->travel_start_ms;

                if (h->extend_time_ms < h->config.min_travel_ms ||
                    h->extend_time_ms > h->config.max_travel_ms)
                {
                    Homing_SetError(h, HOMING_ERR_INVALID_TRAVEL);
                }
                else
                {
                    h->command = ACTUATOR_RETRACT;
                    h->state = HOMING_MEASURE_RETURN;
                    h->state_start_ms = now_ms;
                    h->travel_start_ms = now_ms;
                }
            }
            else
            {
                h->command = ACTUATOR_EXTEND;
            }
            break;

        case HOMING_MEASURE_RETURN:
            if (Homing_Elapsed(now_ms, h->state_start_ms,
                               h->config.seek_timeout_ms))
            {
                Homing_SetError(h, HOMING_ERR_TIMEOUT);
            }
            else if (inputs.left_switch)
            {
                h->retract_time_ms = now_ms - h->travel_start_ms;

                if (h->retract_time_ms < h->config.min_travel_ms ||
                    h->retract_time_ms > h->config.max_travel_ms)
                {
                    Homing_SetError(h, HOMING_ERR_INVALID_TRAVEL);
                }
                else
                {
                    h->command = ACTUATOR_EXTEND;
                    h->state = HOMING_MOVE_MIDDLE;
                    h->state_start_ms = now_ms;
                    h->travel_start_ms = now_ms;
                }
            }
            else
            {
                h->command = ACTUATOR_RETRACT;
            }
            break;

        case HOMING_MOVE_MIDDLE:
        {
            uint32_t middle_time = h->extend_time_ms / 2U;

            if (Homing_Elapsed(now_ms, h->state_start_ms,
                               h->config.seek_timeout_ms))
            {
                Homing_SetError(h, HOMING_ERR_TIMEOUT);
            }
            else if ((uint32_t)(now_ms - h->travel_start_ms) >= middle_time)
            {
                h->command = ACTUATOR_STOP;
                h->state = HOMING_DONE;
            }
            else
            {
                h->command = ACTUATOR_EXTEND;
            }
            break;
        }

        default:
            Homing_SetError(h, HOMING_ERR_INVALID_TRAVEL);
            break;
    }
}




