# Task

As the task was given I have implemented C code that helps an actuator find its reference position and move to the middle position.

I have separeted the code into 3 main files:
- homing.h which contains definitions, states, commands, and parameters.
- homing.c contains the main homing logic.
- main.c simulates the program running on my pc.

I have also created main_stm32.c file where the core controller is designed to be portable to an STM32 application using the STM32 HAL. Since I didn't had an actual STM32 at home and had problem on Wowki STM32 simulator with times I just prepared the code for the integration. 

To test the code I compiled it with GCC compiler, running a simulation, and using separate test programs (.exe-s).

I have also implemented some error handlings and restart function. 

