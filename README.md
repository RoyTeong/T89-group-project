# T89 GROUP PROJECT

### GROUP MEMBERS:
1. Joshua Loh Jun Wei 2203684
2. Phua Tiffany 2202698
3. Lim Xue Er 2203261
4. Clarissa Ow 2202555
5. Roy Teong Ying Jun 2200507

<br>

### OBJECTIVES:
- To create a robot car
- Configuration of infrared sensors
- Incorporation of a PID controller

<br>

### EQUIPMENT:
1. A laptop that has the Pico C/C++ SDK installed
2. Raspberry Pico W
3. L298N Motor Drive Board Module
4. Micro-USB Cable
5. F-F Jumper wires
6. M-F Jumper wires
7. M-M Jumper wires
8. Screws
9. AA batteries
10. Battery Bay Module
11. Line Tracking Sensor Module (IR Sensor)
12. Counting Speed Sensor Module (Magnetometer Sensor)
13. Ultrasonic Distance Sensor Module
14. Ultrasonic Distance Sensor Holder
15. Motor Controller & DC Motor
16. Wheel
17. Sensor Mounting Plate
18. Front Wheel
19. Rear Wheel
20. Car Chasis
21. Dynamo
22. Brackets

<br>

## INTRODUCTION
This project presents students with the task of designing an intelligent autonomous vehicle with two wheels, utilising the Raspberry Pi Pico microcontroller. The vehicle is equipped with a range of sensors and incorporates a PID (Proportional-Integral-Derivative) controller. The main objective of this project is to design and construct an autonomous robotic car capable of efficiently navigating a predetermined course while effectively avoiding obstacles and accurately identifying barcodes through the utilisation of infrared sensors. The incorporation of a Proportional-Integral-Derivative (PID) controller improves the control system of the automobile, guaranteeing seamless and precise movement while navigating and scanning barcodes.

<br>

## BLOCK DIAGRAM
![Block Diagram](https://github.com/RoyTeong/T89-group-project/assets/115625499/7827ba22-0a25-4b2c-a4c8-6cb14bd7643f)

<br>

## DATA FLOW DIAGRAM
![GRP_89 - Dataflow](https://github.com/RoyTeong/T89-group-project/assets/115625499/034b270a-cb9e-4acc-ae71-7451336df16f)

<br>

## FLOWCHART DIAGRAM
![GRP_89 - Flowchart](https://github.com/RoyTeong/T89-group-project/assets/115625499/74272e1f-f188-4b9f-9002-98b4a669e6a1)

<br>

## DRIVERS

### Line Tracking Sensor Module (IR Sensor)
2 sensors are used to locate and track a given line or path on a surface. The sensor can make real-time modifications to a robot's or vehicle's movement by using infrared technology to detect the contrast between the line and its background.

1 sensor is employed for the purpose of reading barcodes. It systematically records the time of the scanning process and discriminates between the white and black segments of the barcode. All these are recorded in binary and concatenated together to decode it as a whole. This ensures data retrieval is accurate and efficient.

### Counting Speed Sensor Module (Magnetometer Sensor)
The magnetometer is a specialized instrument used to measure and track the velocity of objects in motion or their rotational speed.   This sensor utilizes magnetometer technology to detect alterations in magnetic fields linked to motion, facilitating accurate counting and speed computations.  

### Ultrasonic Distance Sensor Module
The Ultrasonic Distance Sensor Module is a technological device that employs ultrasonic waves for the purpose of measuring distances.  It emits ultrasonic pulses and measures the time it takes for the pulses to return after hitting an object.   The sensor precisely calculates the distance to the object by examining the time delay.   Ultrasonic Distance Sensor Modules have extensive applications in various systems and devices for detecting obstacles, aiding in navigation, and sensing proximity.

### Motor Controller & DC Motor
The motor controller functions as the central control unit, overseeing the operation of the DC motor by regulating electrical inputs. They allow for accurate control of motor speed, direction, and torque.   The harmonious interaction between the motor controller and DC motor is crucial for effectively and efficiently providing power to a wide variety of machinery and devices. 

