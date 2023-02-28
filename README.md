# rear_sensor_array

This code runs on bare metal Raspi Pico 

## What it is supposed to do:

Read data from 2 potentiometers, the suspension position sensors on the coilovers of the car.

Read data from the accelerometer of the ACC8700 from Gravitech via i2c protocol.

Read date from the magnetometer of the ACC8700 from Gravitech via i2c protocol.

Normalise data in order to become readable by the user.

Send the data via the CAN bus by using PIO inside the Raspi Pico

# Still in development