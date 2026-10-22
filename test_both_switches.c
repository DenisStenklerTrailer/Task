
#include <stdio.h>
#include "homing.h"

int main(void)
{
    Homing homing;

    HomingConfig config = {
        .seek_timeout_ms = 1000U,
        .min_travel_ms = 100U,
        .max_travel_ms = 8000U
    };

    HomingInputs inputs = {
        .left_switch = true,
        .right_switch = true
    };

    Homing_Init(&homing, &config);

    if (!Homing_Start(&homing, 0U))
    {
        printf("TEST FAILED: could not start homing.\n");
        return 1;
    }

    Homing_Update(&homing, 10U, inputs);

    printf("State: %d\n", homing.state);
    printf("Error: %d\n", homing.error);
    printf("Command: %d\n", homing.command);

    if (homing.state == HOMING_ERROR &&
        homing.error == HOMING_ERR_BOTH_SWITCHES &&
        homing.command == ACTUATOR_STOP)
    {
        printf("TEST PASSED: both switches detected, actuator stopped.\n");
        return 0;
    }

    printf("TEST FAILED: expected both-switch fault.\n");
    return 1;
}
