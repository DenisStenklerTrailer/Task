
#include <stdio.h>
#include <stdint.h>
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

    uint32_t start_time = UINT32_MAX - 49U;
    uint32_t now_ms;

    Homing_Init(&homing, &config);

    if (!Homing_Start(&homing, start_time))
    {
        printf("TEST FAILED: could not start homing.\n");
        return 1;
    }

    /* Simulate timer overflow: elapsed time is 60 ms. */
    now_ms = start_time + 60U;

    Homing_Update(&homing, now_ms, inputs);

    printf("Start time: %lu\n", (unsigned long)start_time);
    printf("Current time: %lu\n", (unsigned long)now_ms);
    printf("State: %d\n", homing.state);
    printf("Error: %d\n", homing.error);
    printf("Command: %d\n", homing.command);

    if (homing.state == HOMING_SEEK_LEFT &&
        homing.error == HOMING_ERR_NONE &&
        homing.command == ACTUATOR_RETRACT)
    {
        printf("TEST PASSED: timer overflow handled correctly.\n");
        return 0;
    }

    printf("TEST FAILED: unexpected state after timer overflow.\n");
    return 1;
}
