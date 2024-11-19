
Have you ever needed to create a simple command api across various stream interfaces?  Perhaps human input from a serial monitor, commands stored on an 
SD Card, or commands coming in over a Bluetooth connection? This project aims to standardize and simplify registering and handling command invocations across various stream based serial interfaces. You could use this to make a remote control vehicle or interact with various IOT devices. Woohoo! 

This repository is a real group effort across a number of contributors over time:

----------------------------------------
1. Using many excellent additions from [qnimble](https://github.com/qnimble)/[qCommand](https://github.com/qnimble/qCommand):
----------------------------------------

[ben-qnimble](https://github.com/ben-qnimble) made some great additions to their fork of this repo. Their changes include case insensitivity,
better interpretation of arguments comming from streams and a bunch more. I'll do my best to add to these significant changes in a useful way.

----------------------------------------
2. Based on StreamCommandParser (This repository which was active in 2020, went dormant and then is active again in 2024):
----------------------------------------
This is a basic stream command parsing library, it is based on Arduino-SerialCommand but allows use of any Stream interface that commands might come through, perhaps Serial, BLE and/or an SD card stream.

StreamCommandParser was written by [jongarrison](https://github.com/jongarrison) and it available [here](https://github.com/jongarrison/StreamCommandParser).

----------------------------------------
3. Based on Arduino-SerialCommand (pre 2020 contributions):
----------------------------------------
A Wiring/Arduino library to tokenize and parse commands received over a serial port. 

The original version of this library was written by [Steven Cogswell](http://husks.wordpress.com) (published May 23, 2011 in his blog post ["A Minimal Arduino Library for Processing Serial Commands"](http://husks.wordpress.com/2011/05/23/a-minimal-arduino-library-for-processing-serial-commands/)).

This is a heavily modified version with smaller footprint and a cleaned up code by Stefan Rado.

