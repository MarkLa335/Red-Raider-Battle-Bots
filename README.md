# Red Raider Battle Bot Project

This project implements a two-part embedded control system using MSP430 microcontrollers. A handheld transmitter reads a two-axis joystick through the ADC and sends the sampled X and Y position values over UART. A receiver MSP430 decodes the transmitted joystick data and drives a dual-motor platform through direction and enable signals.

The project demonstrates analog signal acquisition, UART-based communication, and GPIO-based motor control in an embedded robotics application.

## Project Overview

The system is divided into two main programs:

- **Transmitter / remote controller**: samples joystick X and Y analog voltages using the MSP430 ADC.
- **Receiver / motor controller**: receives joystick data over UART and converts it into forward, backward, left, right, and stop motor actions.

At a high level, the joystick position is converted into digital values, transmitted as bytes, reconstructed by the receiver, and then compared against center and threshold values to determine the commanded motion.

## Features

- Two-axis joystick input using ADC channels.
- UART communication between two MSP430 boards.
- Differential drive motor control.
- Direction control for left and right motors.
- Threshold-based decision logic for movement.
- Modular functions for initialization, movement control, and communication.

## Hardware Used

- 2 MSP430 microcontrollers.
- Two-axis analog joystick module.
- Dual H-bridge motor driver.
- Two DC motors.
- Power supply for logic and motors.
- Jumper wires and supporting breadboard/prototyping hardware.

## Software Structure

### Transmitter

The transmitter code performs the following tasks:

1. Configures joystick pins as analog inputs.
2. Sets up the ADC to read the X and Y joystick axes.
3. Initializes UART for serial transmission.
4. Samples joystick values.
5. Sends the sampled values to the receiver.

### Receiver

The receiver code performs the following tasks:

1. Initializes UART for serial reception.
2. Reconstructs transmitted joystick values from received bytes.
3. Compares the joystick values to center and threshold limits.
4. Drives the motor control pins to command motion.
5. Stops the motors when the joystick returns to the dead-zone.

## Motion Logic

The receiver uses joystick thresholds to determine movement:

- **Forward**: Y value above the center plus threshold.
- **Backward**: Y value below the center minus threshold.
- **Right**: X value above the center plus threshold.
- **Left**: X value below the center minus threshold.
- **Stop**: X and Y values remain within the threshold dead-zone.

## Pin Summary

### Receiver motor pins

- `P2.2` -> Left motor direction input 1
- `P1.4` -> Left motor direction input 2
- `P1.5` -> Right motor direction input 3
- `P1.3` -> Right motor direction input 4
- `P2.0` -> Left motor enable / PWM
- `P2.4` -> Right motor enable / PWM

### Transmitter joystick pins

- `P2.7` -> Joystick X axis analog input
- `P2.6` -> Joystick Y axis analog input

### UART pins

- `P2.0` / `P2.1` used for UART on the MSP430 configuration shown in the code.

> Note: Exact pin mappings and register names depend on the specific MSP430 device used. Some MSP430 families use different UART and ADC register names, so the code may require small changes depending on the selected part.

## Build Notes

- Import the project into Code Composer Studio or another MSP430-compatible toolchain.
- Make sure the selected MSP430 part matches the register names used in the source files.
- Verify UART baud-rate settings match on both boards.
- Confirm ADC pin mappings match the joystick wiring on the chosen MSP430 board.
- Check motor driver wiring before enabling motor outputs.

## Future Improvements

- Add PWM speed control instead of simple enable on/off control.
- Add checksum or framing bytes for more robust UART communication.
- Add joystick calibration and dynamic dead-zone tuning.
- Add support for diagonal movement and proportional steering.
- Add wireless communication hardware if the link is later moved away from direct UART wiring.

## Learning Outcomes

This project is useful for practicing:

- MSP430 GPIO configuration.
- MSP430 ADC setup.
- UART transmission and reception.
- Interrupt-driven data handling.
- Motor driver interfacing.
- Embedded C modular design.

## Notes

This repository contains project code for an embedded motor-control system and may need to be adapted for the exact MSP430 variant, motor driver, and joystick hardware used in the final build.
