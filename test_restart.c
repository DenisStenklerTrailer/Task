
#include <stdio.h>
#include "homing.h"

int main(void)
{
    Homing homing;

    HomingConfig config = {
        .seek_timeout_ms = 100U,
        .min_travel_ms = 10U,
        .max_travel_ms = 1000U
    };

    HomingInputs inputs = {
        .left_switch = false,
        .right_switch = false
    };

    Homing_Init(&homing, &config);

    /* First homing attempt. */
    if (!Homing_Start(&homing, 0U))
    {
        printf("TEST FAILED: first start failed.\n");
        return 1;
    }

    /* Trigger timeout. */
    Homing_Update(&homing, 100U, inputs);

    if (homing.state != HOMING_ERROR ||
        homing.error != HOMING_ERR_TIMEOUT ||
        homing.command != ACTUATOR_STOP)
    {
        printf("TEST FAILED: timeout was not detected.\n");
        return 1;
    }

    printf("First attempt: timeout detected.\n");

    /* Restart after the error. */
    if (!Homing_Start(&homing, 200U))
    {
        printf("TEST FAILED: restart was rejected.\n");
        return 1;
    }

    printf("State after restart: %d\n", homing.state);
    printf("Error after restart: %d\n", homing.error);
    printf("Command after restart: %d\n", homing.command);

    if (homing.state == HOMING_SEEK_LEFT &&
        homing.error == HOMING_ERR_NONE &&
        homing.command == ACTUATOR_RETRACT &&
        homing.state_start_ms == 200U &&
        homing.travel_start_ms == 200U &&
        homing.extend_time_ms == 0U &&
        homing.retract_time_ms == 0U)
    {
        printf("TEST PASSED: homing restarted correctly.\n");
        return 0;
    }

    printf("TEST FAILED: state was not reset correctly.\n");
    return 1;
}
