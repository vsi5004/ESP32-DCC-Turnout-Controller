# ESP32 DCC Turnout Controller

[![Build Status](https://github.com/vsi5004/ESP32-DCC-Turnout-Controller/actions/workflows/main.yml/badge.svg)](https://github.com/vsi5004/ESP32-DCC-Turnout-Controller/actions)

## Overview
I needed a DCC turnout controller that did the following:
* Used hobby-grade servos to move the points
* Could run up to 12 turnouts from one board
* Could switch power to frogs based on point position
* Was made from easily accessible, inexpensive components
* Was simple to configure without having to break out a programming cable or memorize CVs

I didn't find anything commercially available, so I made my own solution, using a few common components I sourced on Amazon:
* ESP32 Wroom development board (with USB C)
* PCA9685 16-Channel 12-Bit PWM Driver board
* 2 Channel 5v Relay module board

The software for this project consists of two parts, the ESP32 code and the React Web Application. The ESP32 has two modes that are controlled via a toggle switch: Setup and DCC Operation. 

### Setup Mode
In its setup mode, the ESP32 acts as a WiFi hotspot with captive portal which redirects anyone connecting to it to a website that allows users to configure the all turnout settings. Data is transmitted via websocket so it is synchronized in (almost) real-time to all connected devices. Data is saved in a text file in ESP32 flash memory so it will be retained when the device is powered off. All signals on the DCC interface are ignored in this mode, although users are able to test the range of the servo throw and polarity of the powered frog via the web interface.

### DCC Operation Mode
Once the toggle switch is moved to DCC Operation, the ESP32 removes the hotspot and only reacts to packets on the DCC interface. This is simplified to only include the turnout ID and whether it should be thrown or closed - all other DCC configuration or programming features will not take any effect on the operation of the turnouts. 


## ESP32 Sketch

By default, the ESP32 sketch will create a SoftAP WiFi hotspot named "ESP32 DCC Turnout Controller" without password. You can change the definition of the SSID and password inside main.cpp.

In Platform IO -> Project Tasks -> esp32dev -> Platform, there are options to build and upload the LITTLEFS filesystem image. Scripts placed in the scripts folder will be called to build the react project into a web app, rename the files to shorten the filenames and compress them using gzip to save on storage and delivery time.

Websocket clients are saved and all will be alerted of any state changes of the ESP32. Each and every websocket messages are expected to update all the state variables of the ESP32 and will be pushed to all clients. The ESP32 will only relay the string to other connected clients and new clients.

## React Web App

The React Web App acts as a websocket client and generates the interface based on what it receives from the ESP32 server.

In the included application, setState is only called on websocket message so that disconnecting from the server will not make the interface update anymore.

## Installation and setup

1. Install [VSCode](https://code.visualstudio.com/) or [VSCodium](https://vscodium.com/)
    * Install the [Platform IO extension](https://platformio.org/install/ide?install=vscode)
2. Install [Node.js](https://nodejs.org/en/) for npm and your extension of choice for JavaScript

## Uploading to ESP32

There are two actions required to upload this to your ESP32.

1. Upload the code (Platform IO -> Project Tasks -> esp32dev -> General -> Upload)
2. Upload the React Web App (Platform IO -> Project Tasks -> esp32dev -> Platform -> Upload Filesystem Image)
