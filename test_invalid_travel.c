
#include <stdio.h>
#include "homing.h"

int main(void)
{
    Homing homing;

    HomingConfig config = {
        .seek_timeout_ms = 1000U,
        .min_travel_ms = 500U,
        .max_travel_ms = 8000U
    };

    HomingInputs inputs = {
        .left_switch = false,
        .right_switch = false
    };

    Homing_Init(&homing, &config);

    if (!Homing_Start(&homing, 0U))
    {
        printf("TEST FAILED: could not start homing.\n");
        return 1;
    }

    /* Reach the left switch after 100 ms. */
    inputs.left_switch = true;
    Homing_Update(&homing, 100U, inputs);

    /* Start moving right. */
    inputs.left_switch = false;
    Homing_Update(&homing, 110U, inputs);

    /* Reach the right switch only 200 ms later. */
    inputs.right_switch = true;
    Homing_Update(&homing, 310U, inputs);

    printf("State: %d\n", homing.state);
    printf("Error: %d\n", homing.error);
    printf("Command: %d\n", homing.command);
    printf("Measured extend time: %lu ms\n",
           (unsigned long)homing.extend_time_ms);

    if (homing.state == HOMING_ERROR &&
        homing.error == HOMING_ERR_INVALID_TRAVEL &&
        homing.command == ACTUATOR_STOP)
    {
        printf("TEST PASSED: invalid travel time detected, actuator stopped.\n");
        return 0;
    }

    printf("TEST FAILED: expected invalid travel error.\n");
    return 1;
}
