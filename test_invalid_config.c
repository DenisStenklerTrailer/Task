
#include <stdio.h>
#include "homing.h"

int main(void)
{
    Homing homing;

    HomingConfig config = {
        .seek_timeout_ms = 1000U,
        .min_travel_ms = 8000U,
        .max_travel_ms = 500U
    };

    Homing_Init(&homing, &config);

    printf("State: %d\n", homing.state);
    printf("Error: %d\n", homing.error);
    printf("Command: %d\n", homing.command);

    if (homing.state == HOMING_ERROR &&
        homing.error == HOMING_ERR_INVALID_CONFIG &&
        homing.command == ACTUATOR_STOP)
    {
        printf("TEST PASSED: invalid configuration rejected.\n");
        return 0;
    }

    printf("TEST FAILED: invalid configuration was not rejected.\n");
    return 1;
}
