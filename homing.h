
#ifndef HOMING_H
#define HOMING_H

#include <stdbool.h>
#include <stdint.h>

/* Actuator command */
typedef enum
{
    ACTUATOR_STOP,
    ACTUATOR_EXTEND,
    ACTUATOR_RETRACT
} ActuatorCommand;

/* States of homing automatin */
typedef enum
{
    HOMING_IDLE,
    HOMING_SEEK_LEFT,
    HOMING_SEEK_RIGHT,
    HOMING_MEASURE_RETURN,
    HOMING_MOVE_MIDDLE,
    HOMING_DONE,
    HOMING_ERROR
} HomingState;

/* Faults */
typedef enum
{
    HOMING_ERR_NONE,
    HOMING_ERR_TIMEOUT,
    HOMING_ERR_BOTH_SWITCHES,
    HOMING_ERR_INVALID_TRAVEL,
    HOMING_ERR_INVALID_CONFIG
} HomingError;

/* Inputs */
typedef struct
{
    bool left_switch;
    bool right_switch;
} HomingInputs;

/* parameters */
typedef struct
{
    uint32_t seek_timeout_ms;
    uint32_t min_travel_ms;
    uint32_t max_travel_ms;
} HomingConfig;

/* homing modul states */
typedef struct
{
    HomingState state;
    HomingError error;
    ActuatorCommand command;

    uint32_t state_start_ms;
    uint32_t travel_start_ms;

    uint32_t extend_time_ms;
    uint32_t retract_time_ms;

    HomingConfig config;
} Homing;

/* */
void Homing_Init(Homing *h, const HomingConfig *config);

bool Homing_Start(Homing *h, uint32_t now_ms);

void Homing_Update(
    Homing *h,
    uint32_t now_ms,
    HomingInputs inputs
);

#endif
