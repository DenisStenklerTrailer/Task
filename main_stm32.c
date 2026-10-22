
#include "STMxx_file.h"
#include "homing.h"

static Homing homing;

static const HomingConfig homing_config = {
    .seek_timeout_ms = 10000U,
    .min_travel_ms = 500U,
    .max_travel_ms = 8000U
};

static uint32_t last_homing_update_ms = 0U;

static void Homing_ApplicationInit(void)
{
    Homing_Init(&homing, &homing_config);
    (void)Homing_Start(&homing, HAL_GetTick());
}

static void Homing_ApplicationTask(void)
{
    uint32_t now_ms = HAL_GetTick();

    if ((uint32_t)(now_ms - last_homing_update_ms) < 10U)
    {
        return;
    }

    last_homing_update_ms = now_ms;

    HomingInputs inputs = {
        .left_switch =
            (HAL_GPIO_ReadPin(LEFT_SWITCH_GPIO_Port,
                              LEFT_SWITCH_Pin) == GPIO_PIN_SET),
        .right_switch =
            (HAL_GPIO_ReadPin(RIGHT_SWITCH_GPIO_Port,
                              RIGHT_SWITCH_Pin) == GPIO_PIN_SET)
    };

    Homing_Update(&homing, now_ms, inputs);

    switch (homing.command)
    {
        case ACTUATOR_EXTEND:
            /* TODO: call your motor driver extend function */
            break;

        case ACTUATOR_RETRACT:
            /* TODO: call your motor driver retract function */
            break;

        case ACTUATOR_STOP:
        default:
            /* TODO: call your motor driver stop function */
            break;
    }
}
