# STM32F407 Motor Controller

This directory contains the embedded controller project recovered from the final prototype archive for the Beijing competition version of the leafy-vegetable cleaning and packaging system.

The firmware coordinates six stepper-motor channels, two continuous-rotation servo channels, two active-low proximity sensors, and two relay-controlled auxiliary outputs.

## Target and Toolchain

- MCU: `STM32F407IGTx`
- Framework: STM32 HAL / CMSIS
- Configuration: STM32CubeMX (`STEEP.ioc`)
- IDE project: Keil MDK-ARM (`MDK-ARM/STEEP.uvprojx`)
- Servo timer: TIM4 channels 1 and 2
- Stepper interface: GPIO step/direction/enable signals

The archived final build produced a HEX image with **0 errors**. The sole warning reported that the original `main.c` did not end with a newline.

## Source Layout

```text
stm32f407-controller/
├── Inc/                 Application and generated headers
├── Src/                 Application and generated source
├── Drivers/             STM32F4 HAL and CMSIS dependencies
├── MDK-ARM/             Keil project and startup source
├── STEEP.ioc            STM32CubeMX hardware configuration
└── README.md
```

The main application logic is in [`Src/main.c`](Src/main.c).

## I/O Map

### Stepper channels

| Channel | Connector | Direction | Enable | Pulse | Pulse period |
|---|---|---:|---:|---:|---:|
| Motor 1 | J25 | PE1 | PE0 | PI5 | 5000 µs |
| Motor 2 | J26 | PI8 | PE4 | PI6 | 1000 µs |
| Motor 3 | J31 | PI11 | PI10 | PI7 | 1000 µs |
| Motor 4 | J32 | PF2 | PF1 | PC9 | 1000 µs |
| Motor 5 | External MD4CH | PE15 | External/not switched | PE13 | 800 µs |
| Motor 6 | External MD4CH | PF11 | External/not switched | PD9 | 300 µs |

Enable outputs for Motors 1–4 are active low. Motor angles are converted to pulse counts using:

```text
pulses = requested angle / (1.8° / microstep subdivision)
```

The current configuration uses `STEP_SUBDIVIDE = 8`.

### Sensors, relays, and servos

| Function | MCU pin | Logic/configuration |
|---|---:|---|
| Proximity Sensor 1 | PH2 | Active low |
| Proximity Sensor 2 | PH3 | Active low |
| Sensor 1 indicator | PE2 | Active low |
| Sensor 2 indicator | PG15 | Active low |
| Relay 1 | PF3 | Active high in the final program |
| Relay 2 | PF4 | Active high in the final program |
| Servo 1 | PD12 / TIM4_CH1 | 50 Hz PWM, inverted timer polarity |
| Servo 2 | PD13 / TIM4_CH2 | 50 Hz PWM, inverted timer polarity |

Both continuous-rotation servos use the following calibrated pulse widths:

| Command | Pulse width |
|---|---:|
| Counterclockwise | 1350 µs |
| Stop | 1500 µs |
| Clockwise | 1650 µs |

## Program Sequence

`Linkage_Run_Once()` executes the following cycle:

1. Switch both relays off and command both servos to stop.
2. Rotate Motor 5 by 90° and then 100° counterclockwise.
3. Run Servo 1 clockwise for 900 ms and stop it.
4. Switch Relay 1 on and run Motor 4 until Sensor 1 becomes active.
5. Switch Relay 1 off and Relay 2 on.
6. Rotate Motor 6 by 360° counterclockwise.
7. Rotate Motor 1 by 90° clockwise.
8. Rotate Motor 2 by 360° counterclockwise.
9. Wait until Sensor 2 becomes active, then switch Relay 2 off.
10. Rotate Motor 3 by 200° clockwise and then 200° counterclockwise.
11. Run Servo 2 counterclockwise, pause, return clockwise, and stop.
12. Return Motor 1 by 90° counterclockwise.
13. Return Servo 1, stop the auxiliary outputs, and begin the next cycle after 3 seconds.

## Building

### Keil MDK

1. Open `MDK-ARM/STEEP.uvprojx` in Keil µVision.
2. Ensure the STM32F4 device support package is installed.
3. Select the `STEEP` target and build the project.

### STM32CubeMX

Open `STEEP.ioc` to inspect or regenerate the GPIO and TIM4 configuration. Preserve code inside CubeMX `USER CODE` sections when regenerating.

## Prototype Limitations and Safety

This is competition-prototype firmware, not production machinery-control software.

- Sensor waits are blocking and currently have no timeout or fault transition.
- Stepper movement is open loop and does not use acceleration ramps or position feedback.
- No emergency-stop, guard interlock, overcurrent handling, or watchdog recovery sequence is implemented in the application logic.
- Relay polarity and continuous-servo neutral pulse widths must be verified on the actual hardware before operation.
- The motor-to-mechanical-module assignment was not recorded in the recovered source and must be checked against the physical wiring.

Keep actuators mechanically disconnected during initial bench testing, verify every output at low power, and add appropriate safety interlocks before operating a physical machine.

## Third-Party Components

STM32 HAL and CMSIS files retain their original copyright and license notices under `Drivers/`. The project-specific application source is published here as part of the technical portfolio; no separate open-source license is granted unless one is added explicitly.
