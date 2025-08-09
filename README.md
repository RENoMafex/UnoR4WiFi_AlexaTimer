# Alexa Timer (shiftOut version)
there is also a [Serial output version](https://github.com/RENoMafex/UnoR4WiFi_AlexaTimer/tree/outputToSerial)!


> [!WARNING]
> This README is still WIP!

## Things you will need:
- Arduino Uno R4 WiFi
- A running instance of ioBroker (ex. on a RaspberryPi)
- 4 shift registers (ex. SN74HC595)
- 4 7-Segment displays
  - Maybe a few hands full of MOSFETs, if you use bigger 7-Segment displays
- A soldering iron with some tin

## Things you will find in this Repo (when finished):
- KiCad files for a circuit board
  - Utilizes SN74HC595N pinouts
  - Utilizes MOSFETs
  - Support for selfmade RGB-LED 7-Segment displays
  - Only through hole soldering (easy for beginners)
- Gerber files for the circuit board
- Code working on an Arduino Uno R4 WiFi, to display a clock or remaining time on a timer from MQTT
- Code for ioBroker, to put the remainder of the timer onto MQTT
- Instructions how to wire everything up

## Setup:
You will find 3 folders inside this repository, `ioBroker`, `platformIO` and `hardware`, with each one `README.md` file inside them, refer to the setup steps provided by these files.

## TODO:
- Design circuit board
