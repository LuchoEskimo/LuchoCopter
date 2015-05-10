# LuchoCopter
The software of a quadcopter.


## 1. Presentation

This repository hosts the software part of a quadcopter project. This quadcopter
won't be the most efficient one: I create all its feature.


## 2. Hardware

Currently, my prototype is based on several hadrwares:
- An ATMEL AVR Atmega328p, the heart of the quadcopter: a modest 8bits microcontroler
  which nevertheless is powerfull enough to drive a quadcopter, with a frequecy
  of 16MHz.
- The MPU-9150, from Invensense: it's an awesome 9 DOF inertial measurement unit
  (IMU) which allows the quadcopter to remain upright.
- An Xbee module, used to communicate with the radio controler through serial
  connection.
- Four electronic speed controllers (ESCs) Turnigy Multistar from Hobbyking: they drive the
  motors.

This quadcopter is also piloted by a DIY remote.

The connections of the Atmega328p are as follows:
- PD0 (RX) to Xbee DOUT
- PD1 (TX) to Xbee DIN
- PD5 to the front right motor
- PD3 to the back right motor
- PD6 to the back left motor
- PB3 to the front left motor
- PC5 (scl) to the SCL pin of the MPU-9150
- PC6 (sda) to the SDA pin of the MPU-9150

## 3. Software

The quadcopter is still a work in progress: all the files I work on are on the
WIP directory. The project is divided in several source files:
- `eon.c`: through its functions, we can mesure the time, using timer 1
- `motors.c`: this file allows us to drive motors through ESCs, using the timers
  0 and 2
- `usart.c`: it provides functions to use the USART module of the Atmega328p
- `i2c.c`: with its functions, we can use the TWI module of the Atmega328p
  as a Master
- `mpu9150.c` (WIP): library which provide the functions to get
  inertial informations about the quadcopter
- `imu.c` (not implemented): provides all the functions needed to obtain
  workable (ie. filtered and analysed) inertial informations
- `pid.c` (not implemented): this library is in charge of the enslavement of the attitude of the
  quadcopter
- `com.c` (not impemented): help us to communicate with the remote
