# Alexa Timer (shiftOut version)
there is also a [Serial output version](https://github.com/RENoMafex/UnoR4WiFi_AlexaTimer/tree/outputToSerial)!


> [!WARNING]
> This README is still WIP!

> [!CAUTION]
> This Repository does not cover everything you will need, to get it going from scratch.
> If you are familiar with ioBroker, arduino or platformIO and soldering, it should be doable, if you are a complete beginner and have trouble setting everything up, don't hesitate to email me at \<schilling.malte@googlemail.com>

## Things you will need:
- Arduino Uno R4 WiFi
- [platformIO][platformIO], either the cli, or the IDE in an editor.
- A running instance of [ioBroker][ioBroker] (ex. on a RaspberryPi)
- 4 shift registers (ex. SN74HC595)
- 4 7-Segment displays
  - Maybe a few hands full of MOSFETs, if you use bigger 7-Segment displays
- A soldering iron with some tin

## Things you will find in this Repo (when finished):
- [KiCad][KiCad] files for a circuit board
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

[platformIO]:https://platformio.org/ "its free, lightweight, crossplatform and open source!"
[ioBroker]: https://www.iobroker.net/ "also free and open source!"
[KiCad]: https://www.kicad.org/ "you guessed it... free and open source!"