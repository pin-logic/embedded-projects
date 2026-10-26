# Ultrasonic Parking Sensor

Arduino Uno project that measures distance with an HC-SR04 ultrasonic sensor and signals proximity with LEDs and a buzzer, like a car parking assistant. A median filter over 5 readings removes spurious spikes.

## Hardware

- Arduino Uno
- HC-SR04 ultrasonic sensor
- Passive or active buzzer
- 3 LEDs (green, yellow, red) with 220 Ω resistors
- Breadboard and jumper wires

## Wiring

| Part | Arduino pin |
|---|---|
| HC-SR04 VCC | 5V |
| HC-SR04 GND | GND |
| HC-SR04 TRIG | D9 |
| HC-SR04 ECHO | D10 |
| Buzzer (+) | D8 |
| Green LED (via 220 Ω) | D4 |
| Yellow LED (via 220 Ω) | D5 |
| Red LED (via 220 Ω) | D6 |

## Behaviour

| Distance | Indication |
|---|---|
| > 100 cm | Green LED, silent |
| 50-100 cm | Yellow LED, slow beeps |
| 20-50 cm | Yellow LED, beeps speed up as you get closer |
| < 20 cm | Red LED, continuous tone |

Distance in cm is also printed on the serial port at 115200 baud.

## Build and flash

```bash
pio run                # build
pio run -t upload      # flash
pio device monitor     # serial output
```

## Status

This project has not been compiled or tested on physical hardware yet. Run `pio run` first and check pin assignments and sensor behaviour on your own setup.
