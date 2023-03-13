# RSA = Rear Sensor Array
<img src="drive.png" width="50%" height="50%">

## Description
This code runs bare metal on Raspi Pi Pico.
<p>It collects data via I2C from an accelerometer / magnetometer</p>
<p>It also measures 2 suspension sensors</p>
<p>Parses data and sends it via CAN BUS</p>
<img src="https://external-content.duckduckgo.com/iu/?u=https%3A%2F%2Fwww.raspberry-pi-geek.de%2Fwp-content%2Fuploads%2F2021%2F06%2FAA_pico-board-white-angle-1_rpi-foundation-768x549.jpg&f=1&nofb=1&ipt=bfd916fa013d9f51f786a68b50ef7c415aa09b8f6b572f866fa8bb1f5e134c21&ipo=images" width="50%" height="50%">
<img src="https://external-content.duckduckgo.com/iu/?u=https%3A%2F%2Ftse4.mm.bing.net%2Fth%3Fid%3DOIP.zxdZ3t1NTV1hwKQ99Cm5xAHaEc%26pid%3DApi&f=1&ipt=5b138c37eeb4fda75f94d02102efc619b8e35c7dc7dba3d2dc258be85d013090&ipo=images" width="50%" height="50%">
<p>This is the accelerometer / magnetometer module which will be on the bottom part of the pcb</p>
<img src="https://www.waveshare.com/media/catalog/product/cache/1/thumbnail/122x122/9df78eab33525d08d6e5fb8d27136e95/s/n/sn65hvd230-can-board-2_5.jpg" width="50%" height="50%">
<p>This is the can trasciever which will be on the upper part on the PCB, on the backside, in order to save some space</p>
<img src="https://snapeda.s3.amazonaws.com/partpics/Stewart%20Connector_SS-60300-002_47590.jpg" width="50%" height="50%">
<p>This is the RJ-45 THD connector which will on the most upper side of the PCB, you can see the holes in which it will reside</p>
<p>I also finished the design of the PCB for this module</p>
<p>It connects via an ethernet connector RJ-45 to the VCC GND and CAN bus of the car</p>
<p>This was a great choice, because the ethernet cables have a small footprint and 8 cables are already bundled toghether</p>
<img src="pcb_img.png" width="50%" height="50%">
<p><i>Please beware of the jokes on the PCB Soldermask. They are greatly appreciated at Formula Student Comptetitions</i></p>

## This module is made for the FSA car of UPB Drive
Find us <a href="https://www.upbdrive.com/">here</a>

## File structure, explained
Here you wil find all the docs, pcb files and firware needed for RSA

## Other?
Nada



