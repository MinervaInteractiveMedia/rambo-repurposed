# rambo-repurposed
# Einsy RAMBo Motor Controller
--- Repurposing the Original Prusa Research Einsy RAMBo as a standalone Arduino-compatible multi-stepper controller using onboard TMC2130 drivers, AccelStepper, and direct SPI control ---


A practical guide for repurposing a Prusa MK3/MK3S [Einsy RAMBo](https://reprap.org/wiki/EinsyRambo) board as a standalone three-axis stepper controller and general I/O platform using standard Arduino code instead of 3D printer firmware.

---

## References

These are the main sources used to verify the board, pin mapping, SPI setup, and driver behavior:

- [Einsy RAMBo overview - RepRap Wiki](https://reprap.org/wiki/EinsyRambo)
- [Einsy RAMBo development pin map - RepRap Wiki](https://reprap.org/wiki/EinsyRambo_development)
- [Einsy RAMBo hardware files - UltiMachine GitHub](https://github.com/ultimachine/Einsy-Rambo)
- [Einsy RAMBo 1.1a schematic PDF](https://github.com/ultimachine/Einsy-Rambo/blob/1.1a/board/Project%20Outputs/Schematic%20Prints_Einsy%20Rambo_1.1a.PDF)
- [Prusa AVR board package](https://github.com/prusa3d/Arduino_Boards)
- [TMC2130 Arduino library](https://github.com/janelia-arduino/TMC2130)
- [AccelStepper documentation](https://www.airspayce.com/mikem/arduino/AccelStepper/)
- [TMC2130 datasheet](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf)
- [RAMBo USB driver for Windows](https://ultimachine.com/pages/rambo-usb-driver)
- [Arduino Mega 2560 documentation](https://docs.arduino.cc/hardware/mega-2560/)
- [ATmega2560 documentation](https://www.microchip.com/en-us/product/atmega2560)

---

## Repository contents

- `einsy_three_motors/einsy_three_motors.ino` - final working Arduino sketch
- `EINSY_RAMBO_STEPPER_GUIDE.md` - combined hardware guide and project README

---

## What this project does

This project repurposes a Prusa MK3/MK3S `Einsy RAMBo` board as a standalone motion controller using the onboard `TMC2130` drivers and standard Arduino code.

The included sketch:

- configures the `TMC2130` drivers over SPI
- enables the `X`, `Y`, and `Z` motor channels
- drives three stepper motors continuously with `AccelStepper`
- uses tested SPI settings that were confirmed working on real hardware

---

## Tested hardware

- Prusa `Einsy RAMBo`
- salvaged Prusa MK3S stepper motors
- 24V DC supply
- Arduino IDE
- `TMC2130` library by Peter Polidoro
- `AccelStepper` by Mike McCauley

---

## What the Einsy RAMBo is

The [Einsy RAMBo](https://reprap.org/wiki/EinsyRambo) is the control board used in the Prusa i3 MK3 and MK3S printers. Internally it combines:

- an **ATmega2560** microcontroller (same as Arduino Mega 2560)
- four integrated **[TMC2130](https://www.trinamic.com/products/integrated-circuits/details/tmc2130/)** stepper drivers
- four **MOSFET switched outputs** (bed, extruder, two fans)
- four **thermistor inputs**
- endstop inputs
- 5V fan outputs with tachometer feedback
- onboard SMPS generating 5V from the 24V rail
- SPI-configurable driver current and microstepping
- native Arduino IDE compatibility
- extension headers exposing additional GPIO

Because it uses the ATmega2560, it programs directly from the Arduino IDE like a standard Arduino Mega. It is effectively a ruggedized, purpose-built motion control Arduino with substantial onboard power handling built in.

The TMC2130 drivers are substantially more capable than typical [A4988](https://reprap.org/wiki/A4988) or DRV8825 modules:

- much quieter (stealthChop)
- higher microstepping resolution (up to 1/256, internally interpolated)
- current control via SPI — no potentiometers
- stall detection (stallGuard2)
- thermal protection and diagnostics

---

## Hardware requirements

**Required:**

- Einsy RAMBo board ([official schematic and hardware files](https://github.com/ultimachine/Einsy-Rambo))
- 24V DC power supply, 5A or higher (Meanwell LRS-50-24 or similar)
- USB-B cable
- NEMA 17 stepper motors
- Motor cables with JST-XH 2.54mm 4-pin connectors

**Recommended:**

- active cooling fan
- ferrules for power wiring
- inline fuse on VIN
- emergency stop switch

---

## Critical: 24V power

> **USB alone does not power the motor drivers.**

The stepper drivers require external 24V connected to the main VIN terminal block. This is the single most common issue when testing salvaged boards.

**Symptoms when 24V is missing:**

- board appears in Arduino IDE
- code uploads successfully
- motors do not move
- drivers appear completely dead

**Fix:** connect a 24V supply before expecting any motor or MOSFET output to work.

**Power rails:**

| Rail | Fuse | Purpose |
|------|------|---------|
| Primary (VIN) | F1 5A | Motors, logic, fans |
| Extruder heater | F2 5A | Extruder MOSFET output |
| Heated bed | F3 15A | Bed MOSFET output |

The onboard SMPS generates 5V from the primary rail for logic and fans. The 5V rail does not come from USB.

---

## Windows USB driver

Windows requires the RAMBo CDC driver before the board appears correctly as a COM device.

**Download:** [RAMBo USB Driver — UltiMachine](https://ultimachine.com/pages/rambo-usb-driver)

After installation the board should appear in Device Manager under **Ports (COM & LPT)** as RAMBo, Arduino Mega 2560, or USB Serial Device.

Linux and macOS have the CDC driver built in — no installation needed.

---

## Arduino IDE setup

No custom board package is required. Select:

- **Tools → Board → Arduino AVR Boards → Arduino Mega or Mega 2560**
- **Tools → Processor → ATmega2560**
- **Tools → Port → matching COM port**

The Einsy behaves exactly like a standard [Arduino Mega 2560](https://docs.arduino.cc/hardware/mega-2560/).

> If you want the full RAMBo board definition (exposes additional extended pins beyond standard Mega numbering), add this URL in Preferences → Additional Boards Manager URLs:
> `https://raw.githubusercontent.com/prusa3d/Arduino_Boards/master/IDE_Board_Manager/package_prusa3d_index.json`
> Then install **Prusa Research AVR Boards** and select **Original Prusa i3 MK3/MK3S Einsy RAMBo**.
> This is optional for basic use but gives access to the extended RamboDigital pins (72–85).

---

## Required libraries

Install via **Sketch → Include Library → Manage Libraries:**

| Library | Author | Purpose |
|---------|--------|---------|
| [TMC2130](https://github.com/janelia-arduino/TMC2130) | Peter Polidoro | SPI configuration of TMC2130 drivers |
| [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) | Mike McCauley | Non-blocking acceleration-based motion |

---

## Pin reference

Full pin documentation: [EinsyRambo Development — RepRap Wiki](https://reprap.org/wiki/EinsyRambo_development)

Full schematic: [Einsy RAMBo 1.1a Schematic PDF](https://github.com/ultimachine/Einsy-Rambo/blob/1.1a/board/Project%20Outputs/Schematic%20Prints_Einsy%20Rambo_1.1a.PDF)

### Stepper driver pins

| Axis | STEP | DIR | ENABLE | CS (SPI) |
|------|------|-----|--------|----------|
| X    | 37   | 49  | 29     | 41       |
| Y    | 36   | 48  | 28     | 39       |
| Z    | 35   | 47  | 27     | 67       |
| E0   | 34   | 43  | 26     | 66       |

### Hardware SPI pins

| Function | Pin |
|----------|-----|
| MOSI     | 51  |
| MISO     | 50  |
| SCK      | 52  |
| SS       | 53  |

### MOSFET outputs

| Output | Pin | Max current | Notes |
|--------|-----|-------------|-------|
| Heated bed | 4 | 15A | switched 24V, F3 fuse |
| Extruder heater | 3 | 5A | switched 24V, F2 fuse |
| Fan 1 | 8 | low power | switched 5V |
| Fan 2 | 6 | low power | switched 5V |

### Thermistor / analog inputs

| Input | Analog pin | Arduino pin |
|-------|-----------|-------------|
| Thermistor 0 | A0 | 54 |
| Thermistor 1 | A1 | 55 |
| Thermistor 2 | A2 | 56 |
| Z Probe thermistor | A3 | 57 |

### Endstop / digital inputs

| Endstop | Pin |
|---------|-----|
| X MIN   | 12  |
| Y MIN   | 11  |
| Z MIN   | 10  |

### StallGuard diagnostic pins (TMC2130 DIAG outputs)

| Axis | DIAG pin |
|------|----------|
| X    | 64       |
| Y    | 69       |
| Z    | 68       |
| E0   | 65       |

---

## Critical SPI configuration

These lines are **mandatory**. If missing, SPI communication with the TMC2130s will fail silently or intermittently.

```cpp
pinMode(53, OUTPUT);
digitalWrite(53, HIGH);

SPI.begin();
SPI.setClockDivider(SPI_CLOCK_DIV16);
```

**Why:**

- Pin 53 is the ATmega2560 hardware SS pin. It must be configured as OUTPUT for SPI master mode — if left as input the chip can drop out of master mode randomly.
- The default SPI clock is too fast for stable communication on many Einsy boards. `SPI_CLOCK_DIV16` slows it to a reliable rate.

**Symptoms of incorrect SPI setup:**

- motors buzz but do not rotate
- random driver initialization failures
- intermittent or partial movement
- complete driver silence

---

## Motor connector pinout

Motor connectors are **JST-XH 2.54mm 4-pin**. Original MK3/MK3S motors plug directly into the board with no rewiring.

For third-party motors, identify coil pairs with a multimeter (coil pairs have ~1–5Ω between them). Incorrect coil pairing causes vibration without rotation.

---

## Working base sketch

The sketch below is the final tested version from `einsy_three_motors/einsy_three_motors.ino`.

It drives `X`, `Y`, and `Z` continuously and was confirmed working on an Einsy RAMBo with salvaged MK3S NEMA 17 motors.

```cpp
#include <Arduino.h>
#include <SPI.h>
#include <TMC2130.h>
#include <AccelStepper.h>

// X axis
const int CS_A   = 41;  const int EN_A   = 29;
const int STEP_A = 37;  const int DIR_A  = 49;

// Y axis
const int CS_B   = 39;  const int EN_B   = 28;
const int STEP_B = 36;  const int DIR_B  = 48;

// Z axis
const int CS_C   = 67;  const int EN_C   = 27;
const int STEP_C = 35;  const int DIR_C  = 47;

TMC2130 driverA;
TMC2130 driverB;
TMC2130 driverC;

AccelStepper motorA(AccelStepper::DRIVER, STEP_A, DIR_A);
AccelStepper motorB(AccelStepper::DRIVER, STEP_B, DIR_B);
AccelStepper motorC(AccelStepper::DRIVER, STEP_C, DIR_C);

void setupDriver(TMC2130 &driver, int cs, int en) {
  pinMode(en, OUTPUT);
  digitalWrite(en, HIGH);       // disable during setup
  delay(50);
  driver.setup(cs);
  delay(100);
  driver.initialize();
  delay(100);
  driver.setAllCurrentValues(70, 35, 70);  // run%, hold%, holdDelay%
  driver.setMicrostepsPerStep(16);
  driver.enableStealthChop();
  delay(50);
  digitalWrite(en, LOW);        // enable
  delay(50);
}

void setup() {
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  pinMode(STEP_A, OUTPUT);  pinMode(DIR_A, OUTPUT);
  pinMode(STEP_B, OUTPUT);  pinMode(DIR_B, OUTPUT);
  pinMode(STEP_C, OUTPUT);  pinMode(DIR_C, OUTPUT);

  delay(500);

  setupDriver(driverA, CS_A, EN_A);
  setupDriver(driverB, CS_B, EN_B);
  setupDriver(driverC, CS_C, EN_C);

  motorA.setMaxSpeed(3200);  motorA.setAcceleration(800);  motorA.moveTo(1000000);
  motorB.setMaxSpeed(3200);  motorB.setAcceleration(800);  motorB.moveTo(1000000);
  motorC.setMaxSpeed(3200);  motorC.setAcceleration(800);  motorC.moveTo(1000000);
}

void loop() {
  motorA.run();
  motorB.run();
  motorC.run();
}
```

---

## Code walkthrough

### Includes

- `Arduino.h` provides the core Arduino functions
- `SPI.h` is required for TMC2130 driver communication
- `TMC2130.h` exposes current, microstepping, and mode configuration
- `AccelStepper.h` generates smooth non-blocking step pulses

### Driver setup

The `setupDriver()` helper keeps initialization consistent across all three axes:

- sets the enable pin as an output
- disables the driver during setup
- starts SPI communication using the axis chip-select pin
- initializes the driver
- applies current and microstepping settings
- enables `stealthChop`
- re-enables the driver

### Motion settings

Each motor is configured with:

- `setMaxSpeed(3200)`
- `setAcceleration(800)`
- `moveTo(1000000)`

At `1/16` microstepping on a 200-step motor, `3200` steps per second is roughly `1 revolution per second`.

### Main loop

The loop only calls:

```cpp
motorA.run();
motorB.run();
motorC.run();
```

This allows `AccelStepper` to keep all three motors moving with acceleration control and without blocking the rest of the sketch.

---

## Motor current

Configured via `driver.setAllCurrentValues(run, hold, holdDelay)` as a percentage of the driver's full scale. Full scale on the Einsy RAMBo is approximately **800 mA RMS** (maximum 940 mA per TMC2130 spec).

| Value | Approx RMS |
|-------|------------|
| 50    | ~400 mA    |
| 70    | ~560 mA    |
| 100   | ~800 mA    |

MK3S NEMA 17s operate comfortably between 400–600 mA RMS. Start low and raise only if motors lose steps under load.

> Do not exceed 940 mA RMS. Without active airflow, sustained loads above ~700 mA may trigger thermal throttling or shutdown.

---

## Microstepping

Configured with `driver.setMicrostepsPerStep()`. Valid values: 1, 2, 4, 8, 16, 32, 64, 128, 256.

At 16 microsteps, a standard 200-step NEMA 17 requires **3200 steps per revolution**.

The TMC2130 internally interpolates to 256 microsteps regardless of the configured value, so lower settings still produce smooth motion.

---

## StealthChop vs SpreadCycle

| Mode | Enable | Character |
|------|--------|-----------|
| stealthChop | `driver.enableStealthChop()` | Very quiet, smooth, lower torque at speed |
| spreadCycle | default (omit enableStealthChop) | Louder, more torque, better high-speed stability |

stealthChop is ideal for installations, instruments, and quiet environments. For heavier loads, faster motion, or robotics, spreadCycle is more reliable.

See the [TMC2130 datasheet](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf) for full detail on chopper modes.

---

## Speed reference

AccelStepper uses **steps per second**. At 16 microsteps on a 200-step motor:

| Steps/sec | Speed |
|-----------|-------|
| 800       | 0.25 rev/sec |
| 1600      | 0.5 rev/sec  |
| 3200      | 1 rev/sec    |
| 6400      | 2 rev/sec    |

- Use `motor.run()` with `setAcceleration()` for ramped motion
- Use `motor.runSpeed()` with `setSpeed()` for constant velocity with no ramp

---

## MOSFET outputs

The board has four MOSFET-switched outputs usable for any resistive or inductive DC load — not just heaters and fans.

**Practical uses:**

- solenoids
- relays
- DC motors (low current, non-reversing)
- LED strips
- electromagnets
- pneumatic valves
- heating elements

**Usage (Arduino digitalWrite):**

```cpp
// Bed heater output — pin 4, up to 15A switched 24V
pinMode(4, OUTPUT);
digitalWrite(4, HIGH);   // on
digitalWrite(4, LOW);    // off

// Extruder heater output — pin 3, up to 5A switched 24V
pinMode(3, OUTPUT);
digitalWrite(3, HIGH);

// Fan outputs — pins 6 and 8, switched 5V, low current
pinMode(8, OUTPUT);
digitalWrite(8, HIGH);
```

**PWM dimming / speed control:**

Pins 3, 6, and 8 support PWM. The bed output (pin 4) is also PWM-capable (OC0B on the ATmega).

```cpp
analogWrite(3, 128);   // 50% duty cycle on extruder output
analogWrite(8, 200);   // variable fan speed
```

> The fan outputs are **5V switched**, not 24V. Do not expect them to drive high-current 24V loads. Use the bed or extruder outputs for that.

> For inductive loads (solenoids, relays, motors) add a flyback diode across the load if not already present — the onboard MOSFETs do not include them.

---

## Thermistor inputs

The board has four analog thermistor inputs usable as general-purpose analog voltage inputs.

| Input | Arduino analog pin | Arduino digital pin |
|-------|--------------------|---------------------|
| T0    | A0                 | 54                  |
| T1    | A1                 | 55                  |
| T2    | A2                 | 56                  |
| Z Probe / T3 | A3          | 57                  |

Each input has a pull-up resistor to 5V forming a voltage divider with whatever you connect. This is standard NTC thermistor circuit topology.

**For actual thermistors:**

```cpp
int raw = analogRead(A0);   // 0–1023
float voltage = raw * (5.0 / 1023.0);
// convert to temperature using your thermistor's B-value curve
```

**For general analog voltage sensing** (0–5V signals, potentiometers, light sensors, pressure sensors, etc.):

```cpp
int value = analogRead(A1);
```

Note the onboard pull-up resistor — for sensors with their own output driver this is fine; for passive sensors the divider formed by the pull-up will affect readings. Consult the schematic for exact resistor values.

---

## Endstops and digital inputs

The three endstop inputs (X MIN, Y MIN, Z MIN) are general-purpose digital inputs with the standard JST-XH 2-pin connector.

| Input | Pin |
|-------|-----|
| X MIN | 12  |
| Y MIN | 11  |
| Z MIN | 10  |

```cpp
pinMode(12, INPUT_PULLUP);
bool triggered = (digitalRead(12) == LOW);
```

**Practical uses:**

- limit switches
- door/position sensors
- buttons and toggles
- optical interrupters
- any digital logic input (3.3V or 5V)

The Z probe connector (4-pin) also exposes an analog input (A3 / pin 57) alongside GND and 5V, making it useful for analog proximity sensors or load cells.

---

## Fan outputs

The two fan connectors are **3-pin JST-XH** outputting switched **5V** (not 24V). The third pin on each connector is a **tachometer input** back to the MCU.

| Fan | PWM pin | Tach pin |
|-----|---------|----------|
| Fan 1 | 8 | varies — see schematic |
| Fan 2 | 6 | varies — see schematic |

The 5V rail comes from the onboard SMPS and is only active when 24V is connected.

**Suitable fans:**

- [Noctua 5V series](https://noctua.at/en/noctua-expands-5v-fan-line-up-2018)
- [Sunon 5V 40mm](https://www.digikey.com/product-detail/en/sunon-fans/ME40100V1-000U-G99/259-1707-ND/)

**For non-fan 5V loads** the fan outputs can drive any 5V logic-level device or low-current peripheral — LED indicators, small relays, signal outputs.

---

## Tachometer feedback

The 3-pin fan connectors include a tachometer (RPM feedback) line from the fan back to the ATmega. This is a standard open-collector pulse output — the fan sends 2 pulses per revolution.

```cpp
// Basic tach read using interrupts — adjust pin to match your fan connector
volatile int tachCount = 0;

void tachISR() {
  tachCount++;
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), tachISR, FALLING);
}

void loop() {
  // RPM = (tachCount / 2) * (60 / elapsed_seconds)
}
```

Useful for confirming fans are actually spinning in an installation, or for closed-loop speed control.

---

## StallGuard - sensorless load detection

The TMC2130 includes stallGuard2, which measures motor load via back-EMF and can detect stalls without a physical endstop. This is how the MK3 does sensorless homing.

**Practical uses beyond homing:**

- detecting mechanical obstruction in a kinetic work
- triggering a response when a motor stalls under load
- event-score triggering based on physical resistance
- replacing limit switches in space-constrained builds

The DIAG pins output a signal when a stall is detected:

| Axis | DIAG pin |
|------|----------|
| X    | 64       |
| Y    | 69       |
| Z    | 68       |
| E0   | 65       |

```cpp
// Enable stallGuard on a driver
driver.setStallGuardThreshold(10);   // lower = more sensitive
// Read DIAG pin
pinMode(64, INPUT);
bool stalled = (digitalRead(64) == HIGH);
```

StallGuard requires tuning per motor and load — start with the [TMC2130 datasheet](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf) section on stallGuard2 configuration.

---

## Serial communication

The ATmega2560 has **four hardware UART ports**, all accessible on the Einsy.

| Port | TX | RX | Notes |
|------|----|----|-------|
| Serial (USB) | 1 | 0 | connected to USB via ATmega32u2 bridge |
| Serial1 | 18 | 19 | available on extension headers |
| Serial2 | 16 | 17 | available on extension headers |
| Serial3 | 14 | 15 | available on extension headers |

**Practical uses:**

- Serial1/2/3 for talking to secondary microcontrollers (ESP32, Teensy, etc.)
- MIDI input/output via Serial1 (with optoisolator on RX)
- GPS modules
- Bluetooth serial modules
- DMX512 (with RS485 driver on Serial1)
- OSC over Serial bridge to a host computer

```cpp
Serial1.begin(31250);   // MIDI baud rate
Serial1.write(0x90);    // note on, channel 1
```

---

## I2C

I2C is available on the standard ATmega2560 pins:

| Function | Pin |
|----------|-----|
| SDA      | 20  |
| SCL      | 21  |

These are exposed on the P3 extension header.

**Practical uses:**

- I2C sensor arrays (IMUs, barometers, ToF distance sensors)
- OLED displays
- I2C GPIO expanders (add more outputs via PCF8574 etc.)
- DACs and ADCs
- real-time clock modules

```cpp
#include <Wire.h>

Wire.begin();
Wire.beginTransmission(0x68);   // example: MPU6050 IMU
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission();
```

---

## Extension headers P1 P2 P3 J19

The Einsy has four expansion headers exposing additional GPIO, SPI, I2C, and UART pins beyond the main connectors. These are primarily intended for LCD panels and SD cards in printer configurations, but are fully usable as general I/O.

Full pin mapping: [EinsyRambo Development — Digital Pin Locations](https://reprap.org/wiki/EinsyRambo_development#Digital_Pin_Locations)

**P1** — LCD interface (SPI-based display, encoder, SD card detect, beeper)
**P2** — Secondary LCD / SD card signals
**P3** — Mixed: I2C (pins 20/21), UART2 (pins 16/17), analog (pin 62)
**J19** — Mixed: SPI bus access, UART1 (pins 18/19), additional GPIO

These headers use 0.1" (2.54mm) pin pitch and are compatible with standard jumper wires and ribbon cables.

**P1 notable pins:**

| Function | Pin |
|----------|-----|
| LCD D4   | 59  |
| LCD D5   | 70  |
| LCD D6   | 85  |
| LCD D7   | 71  |
| LCD RS   | 82  |
| LCD Enable | 61 |
| BTN Click | 9  |
| Beeper   | 84  |

For non-LCD use, all of these are available as standard digital I/O.

---

## Analog inputs

Beyond the four thermistor inputs, the ATmega2560 has a full 16-channel ADC (A0–A15, pins 54–69). Several channels are exposed on extension headers.

All analog inputs operate at 0–5V, 10-bit resolution (0–1023).

**Exposed analog channels:**

| Channel | Arduino analog | Digital pin | Location |
|---------|---------------|-------------|----------|
| A0 | A0 | 54 | Thermistor T0 |
| A1 | A1 | 55 | Thermistor T1 |
| A2 | A2 | 56 | Thermistor T2 |
| A3 | A3 | 57 | Z Probe |
| A8 | A8 | 62 | P3 header |
| A14 | A14 | 68 | Z DIAG |
| A15 | A15 | 69 | Y DIAG |

**Practical uses:**

- potentiometers for speed/position control
- FSR (force sensitive resistors)
- light-dependent resistors
- microphone envelopes (with preamp)
- CV input from modular synthesizers (0–5V range)
- strain gauges (with instrumentation amp)

---

## Power rails and fuses

| Fuse | Rating | Supplies |
|------|--------|---------|
| F1   | 5A     | Motors, logic, 5V SMPS, fans |
| F2   | 5A     | Extruder heater MOSFET output |
| F3   | 15A    | Heated bed MOSFET output |

All fuses are standard **ATO automotive blade fuses**, widely available. Keep spares.

The onboard SMPS generates 5V logic power from the primary rail — the board will not function correctly (logic unstable, SPI failures) if F1 is blown even if USB is connected.

The bed output (F3, 15A) can drive substantial DC loads — lighting rigs, large solenoid arrays, DC motors with an H-bridge — as long as the load is resistive or protected with flyback diodes.

---

## Troubleshooting

### Motors do not move

Check these first:

- `24V` supply is connected
- motor plugs are seated correctly
- `TMC2130` and `AccelStepper` libraries are installed
- the correct COM port is selected
- the SPI setup lines are present:
  - `pinMode(53, OUTPUT);`
  - `SPI.setClockDivider(SPI_CLOCK_DIV16);`

### Motors buzz but do not rotate

Possible causes:

- incorrect motor coil pairing
- bad motor connector wiring
- unstable SPI communication with the driver

### One axis works and another does not

Check:

- the motor itself on a known-good connector
- the axis-specific `STEP`, `DIR`, `EN`, and `CS` pin mapping
- connector damage on salvaged hardware

---

## Known stability issues

**Avoid calling `driver.getStatus()` inside fast loops while printing to Serial.**

On the ATmega2560, concurrent SPI reads and UART output can corrupt serial data and destabilize motion timing. This is a hardware-level timing constraint.

Safe practice:

- read driver status infrequently and only when motors are idle
- snapshot `motor.currentPosition()` (pure software, no SPI) for position logging
- avoid `Serial.print()` inside tight step loops at high speeds
- if you need both motion and serial output, use a slower reporting interval (every 2–3 seconds minimum)

---

## Thermal limits

The onboard TMC2130s are not heavily heatsinked. For sustained multi-motor operation:

- add active airflow over the board
- avoid enclosed unventilated enclosures
- monitor driver temperature if running near current limits
- ~700 mA RMS per driver is a practical continuous limit without forced air

---

## Additional resources

- [EinsyRambo — RepRap Wiki](https://reprap.org/wiki/EinsyRambo)
- [EinsyRambo Development / Full Pin Map — RepRap Wiki](https://reprap.org/wiki/EinsyRambo_development)
- [Einsy RAMBo Schematic PDF](https://github.com/ultimachine/Einsy-Rambo/blob/1.1a/board/Project%20Outputs/Schematic%20Prints_Einsy%20Rambo_1.1a.PDF)
- [Einsy RAMBo Hardware Files — UltiMachine GitHub](https://github.com/ultimachine/Einsy-Rambo)
- [Prusa Arduino Board Definitions — GitHub](https://github.com/prusa3d/Arduino_Boards)
- [TMC2130 Library by Peter Polidoro — GitHub](https://github.com/janelia-arduino/TMC2130)
- [AccelStepper Documentation](https://www.airspayce.com/mikem/arduino/AccelStepper/)
- [TMC2130 Datasheet — Trinamic](https://www.trinamic.com/fileadmin/assets/Products/ICs_Documents/TMC2130_datasheet.pdf)
- [RAMBo USB Driver — UltiMachine](https://ultimachine.com/pages/rambo-usb-driver)
- [Arduino Mega 2560 Documentation](https://docs.arduino.cc/hardware/mega-2560/)
- [ATmega2560 Datasheet — Microchip](https://www.microchip.com/en-us/product/atmega2560)

---

*Guide produced from hands-on testing with a salvaged Prusa MK3S Einsy RAMBo and three NEMA 17 motors. Pin mapping verified against the RepRap wiki schematic and confirmed working. Expanded sections based on board schematic and ATmega2560 datasheet.*
