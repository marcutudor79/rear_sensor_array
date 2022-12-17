# rear_sensor_array

This code runs on bare metal raspi pico 

What it is supposed to do:
Read data from MPU6050 module
Normalise it from raw to m/s^2

Read data from 2 potentiometers
(maybe normalise it, idk)

Send data via CAN

What it currently does:
Read data from MPU6050 module (4g sensitivity)
Normalise it to m/s^2
Sends it via USB
