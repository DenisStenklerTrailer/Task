
#include <stdio.h>
#include "homing.h"

#define SIM_STEP_MS       10U
#define LEFT_POSITION     0.0f
#define RIGHT_POSITION   100.0f
#define INITIAL_POSITION  50.0f

/* Different actuator speeds in the two directions. */
#define EXTEND_SPEED      (100.0f / 3000.0f)
#define RETRACT_SPEED     (100.0f / 4000.0f)

int main(void)
{
    Homing homing;

    HomingConfig config = {
        .seek_timeout_ms = 10000U,
        .min_travel_ms = 100U,
        .max_travel_ms = 8000U
    };

    HomingInputs inputs = { false, false };

    uint32_t now_ms;
    float position = INITIAL_POSITION;

    Homing_Init(&homing, &config);

    if (!Homing_Start(&homing, 0U))
    {
        printf("Failed to start homing.\n");
        return 1;
    }

    printf("Starting position: %.1f%%\n", position);
    printf("Extend speed: %.5f %%/ms\n", EXTEND_SPEED);
    printf("Retract speed: %.5f %%/ms\n\n", RETRACT_SPEED);

    for (now_ms = 0U; now_ms <= 30000U; now_ms += SIM_STEP_MS)
    {
        /* Simulate the end switches from the actual position. */
        inputs.left_switch = (position <= LEFT_POSITION);
        inputs.right_switch = (position >= RIGHT_POSITION);

        Homing_Update(&homing, now_ms, inputs);

        /* Simulate actuator movement during the next time step. */
        if (homing.command == ACTUATOR_EXTEND)
        {
            position += EXTEND_SPEED * SIM_STEP_MS;

            if (position > RIGHT_POSITION)
            {
                position = RIGHT_POSITION;
            }
        }
        else if (homing.command == ACTUATOR_RETRACT)
        {
            position -= RETRACT_SPEED * SIM_STEP_MS;

            if (position < LEFT_POSITION)
            {
                position = LEFT_POSITION;
            }
        }

        if (now_ms % 500U == 0U)
        {
            printf("t=%5lu ms | state=%d | command=%d | "
                   "position=%6.2f%% | error=%d\n",
                   (unsigned long)now_ms,
                   homing.state,
                   homing.command,
                   (double)position,
                   homing.error);
        }

        if (homing.state == HOMING_DONE ||
            homing.state == HOMING_ERROR)
        {
            printf("\nHoming finished at %lu ms\n",
                   (unsigned long)now_ms);
            printf("Final position: %.2f%%\n", (double)position);
            printf("Final state: %d\n", homing.state);
            printf("Final error: %d\n", homing.error);

            if (homing.state == HOMING_DONE &&
                position >= 49.0f && position <= 51.0f)
            {
                printf("TEST PASSED: actuator reached the midpoint.\n");
                return 0;
            }

            printf("TEST FAILED: actuator did not reach the midpoint.\n");
            return 1;
        }
    }

    printf("TEST FAILED: simulation timed out.\n");
    return 1;
}
