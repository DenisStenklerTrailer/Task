
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
        .left_switch = false,
        .right_switch = false
    };

    uint32_t now_ms;

    Homing_Init(&homing, &config);

    if (!Homing_Start(&homing, 0U))
    {
        printf("TEST FAILED: could not start homing.\n");
        return 1;
    }

    for (now_ms = 0U; now_ms <= 1500U; now_ms += 10U)
    {
        /* Neither switch activates. */
        Homing_Update(&homing, now_ms, inputs);

        if (homing.state == HOMING_ERROR)
        {
            printf("Error detected at %lu ms\n",
                   (unsigned long)now_ms);
            printf("State: %d\n", homing.state);
            printf("Error: %d\n", homing.error);
            printf("Command: %d\n", homing.command);

            if (homing.error == HOMING_ERR_TIMEOUT &&
                homing.command == ACTUATOR_STOP)
            {
                printf("TEST PASSED: timeout stops actuator.\n");
                return 0;
            }

            printf("TEST FAILED: unexpected error or command.\n");
            return 1;
        }
    }

    printf("TEST FAILED: timeout was not detected.\n");
    return 1;
}
