# RSA = Rear Sensor Array
<img src="drive.png" width="50%" height="50%">

## Description
This code runs bare metal on Raspi Pi Pico.
<p>It collects data via I2C from an accelerometer / magnetometer</p>
<p>It also measures 2 suspension sensors</p>
<p>Parses data and sends it via CAN BUS</p>
<img src="https://external-content.duckduckgo.com/iu/?u=https%3A%2F%2Fwww.raspberry-pi-geek.de%2Fwp-content%2Fuploads%2F2021%2F06%2FAA_pico-board-white-angle-1_rpi-foundation-768x549.jpg&f=1&nofb=1&ipt=bfd916fa013d9f51f786a68b50ef7c415aa09b8f6b572f866fa8bb1f5e134c21&ipo=images" width="50%" height="50%">

<p>I also finished the design of the PCB for this module</p>
<p>It connects via an ethernet connector to the VCC GND and CAN bus of the car</p>
<p>This was a great choice, because the ethernet cables have a small footprint and 8 cables are already bundled toghether</p>
<img src="pcb_img.png">

## This module is made for the FSA car of UPB Drive
Find us <a href="https://www.upbdrive.com/">here</a>

## File structure, explained
Here you wil find all the docs, pcb files and firware needed for RSA

## Other?
Nada



