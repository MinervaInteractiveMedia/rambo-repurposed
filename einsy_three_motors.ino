#include <Arduino.h>
#include <SPI.h>
#include <TMC2130.h>
#include <AccelStepper.h>

// ======================
// X Y Z DRIVER PINS
// ======================

// X axis
const int CS_A   = 41;
const int EN_A   = 29;
const int STEP_A = 37;
const int DIR_A  = 49;

// Y axis
const int CS_B   = 39;
const int EN_B   = 28;
const int STEP_B = 36;
const int DIR_B  = 48;

// Z axis
const int CS_C   = 67;
const int EN_C   = 27;
const int STEP_C = 35;
const int DIR_C  = 47;

// ======================
// DRIVER OBJECTS
// ======================

TMC2130 driverA;
TMC2130 driverB;
TMC2130 driverC;

// ======================
// STEPPER OBJECTS
// ======================

AccelStepper motorA(AccelStepper::DRIVER, STEP_A, DIR_A);
AccelStepper motorB(AccelStepper::DRIVER, STEP_B, DIR_B);
AccelStepper motorC(AccelStepper::DRIVER, STEP_C, DIR_C);

// ======================
// DRIVER SETUP
// ======================

void setupDriver(TMC2130 &driver, int cs, int en) {

  pinMode(en, OUTPUT);

  // disable driver during setup
  digitalWrite(en, HIGH);

  delay(50);

  driver.setup(cs);

  delay(100);

  driver.initialize();

  delay(100);

  // run current, hold current, delay
  driver.setAllCurrentValues(70, 35, 70);

  // 1/16 microstepping
  driver.setMicrostepsPerStep(16);

  // quiet motion mode
  driver.enableStealthChop();

  delay(50);

  // enable driver
  digitalWrite(en, LOW);

  delay(50);
}

// ======================
// SETUP
// ======================

void setup() {

  // SPI master config
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);

  SPI.begin();

  // safer SPI speed for TMC2130
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  // explicit outputs
  pinMode(STEP_A, OUTPUT);
  pinMode(DIR_A, OUTPUT);

  pinMode(STEP_B, OUTPUT);
  pinMode(DIR_B, OUTPUT);

  pinMode(STEP_C, OUTPUT);
  pinMode(DIR_C, OUTPUT);

  delay(500);

  // initialize drivers
  setupDriver(driverA, CS_A, EN_A);
  setupDriver(driverB, CS_B, EN_B);
  setupDriver(driverC, CS_C, EN_C);

  // ======================
  // MOTOR SETTINGS
  // ======================

  motorA.setMaxSpeed(3200);
  motorA.setAcceleration(800);
  motorA.moveTo(1000000);

  motorB.setMaxSpeed(3200);
  motorB.setAcceleration(800);
  motorB.moveTo(1000000);

  motorC.setMaxSpeed(3200);
  motorC.setAcceleration(800);
  motorC.moveTo(1000000);
}

// ======================
// LOOP
// ======================

void loop() {

  motorA.run();
  motorB.run();
  motorC.run();
}