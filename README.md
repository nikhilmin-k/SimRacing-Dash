# SimRacing-Dash
ESP32 based dashboard code

Pinout:

Seven Segment Pins (A to H): {32,2,4,5,12,13,14,33} <-Pulls low (Common Anode)

10 Segment Pins (left to rightmost LED): {15,21,22,23,18,25,26,27} <-Pulls high

### Instructions:

Build AC_Arduino from this repo: https://github.com/kletellier/Arduino-Assetto-Corsa-Dashboard

Only limitation is to run Assetto Corsa before running AC_Arduino.exe.  

Wiring supports 1 seven segment display, 1 servo motor as a tachometer, and 1 10 segment display.  

Adding a pushbutton to pull the Anode of the seven segment display to ground can be useful if the device does not respond.  

### Parts Used:
Seven Segment: 5161BS
10 Segment: YSLB-10251B5-10
Servo: SG90 Microservo
