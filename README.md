# AutoKey

Automatic Keystrokes (AutoKey) is a project with the aim of automating text copying in closed systems

## Requirements

- Arduino Leonardo
- USB UART
- USBasp Programmer

## Install USB UART driver

This device allows connection to the COM/Serial port between the Arduino and the machine that will send the commands for automatic text copying

### 1. Connect USB UART to PC

### 2. Check if the driver is installed in Device Manager

### 3. If not, install driver 

https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

## Install USBasp driver

This device will allow you to upload the Autokey program to the Arduino, configure the core files and eliminate the bootloader

### 1. Connect USBasp to PC

### 2. Check if the driver is installed in Device Manager

### 3. If not, install Zadig 

https://zadig.akeo.ie

### 4. Select USBasp device, select WinUSB driver and Install Driver

<img src="images\zadig.png" alt="zadig"/>

## Set up Arduino

### 1. Connect USB UART to Arduino

<img src="images\usbuart.png" alt="usbuart" style="width:600px;"/>

### 2. Connect USBasp to Arduino

<img src="images\usbasp.png" alt="usbasp" style="width:600px;"/>

### 3. Remove CDC Class in Arduino Core

Remove CDC Class disables the Arduino Serial Port, the Arduino will only be detected as a HID device (keyboard or mouse)

#### 3.1. Open USBDesc.h

On Windows the file is commonly located in:

    C:\Users\{USER}\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.6\cores\arduino\USBDesc.h

#### 3.2. Uncomment line 35

    //#define CDC_DISABLED

### 4. Upload program via ICSP

#### 4.1. Connect Arduino Leonardo in PC using USBasp

#### 4.2. Open main.ino in Arduino IDE

https://github.com/smenaaliaga/autokey/main/main.ino

### 4.3. Select board: Arduino Leonardo

-> Tools -> Board -> Arduino AVR Boards -> Arduino Leonardo

<img src="images\arduinoide-leonardo.png" alt="arduinoide-leonardo"/>

#### 4.4. Select Programmer: USBasp

-> Tools -> Programmer -> USBasp

<img src="images\arduinoide-usbasp.png" alt="arduinoide-usbasp"/>

#### 4.5. Upload code using programmer

-> Sketch -> Upload Using Programmer

<img src="images\arduinoide-programmer.png" alt="arduinoide-programmer"/>

### 4.6. Done!

<img src="images\arduinoide-done-programmer.png" alt="arduinoide-done-programmer"/>

## How to use

### 1. Connect USB UART to controller PC

### 2. Connect USB Arduino to target PC

### 3. Install and use AutoKey WebUI

https://github.com/smenaaliaga/autokey_webui

## Upload Leonardo's bootloader and CDC Class

### 1. Open USBDesc.h and comment line

    #define CDC_DISABLED

### 2. Connect USBasp to Arduino

### 3. Connect USBasp to PC

### 4. Open Arduino IDE and select board: Arduino Leonardo

### 5. Select Programmer: USBasp

### 6. Start burning the bootloader

-> Tools -> Burn Bootloader

<img src="images\arduinoide-bootloader.png" alt="arduinoide-bootloader"/>

### 7. Done!

<img src="images\arduinoide-done-bootloader.png" alt="arduinoide-done-bootloader"/>

## References: 
- https://www.instructables.com/Fixing-Bootloader-on-Arduino-Leonardo-ProMicro-Wit/
- https://www.fischl.de/usbasp/
- https://electronics.stackexchange.com/questions/416714/avrdude-does-not-recognize-usbasp-device/417509#417509
- https://forum.arduino.cc/t/removing-the-bootloader/534266
- https://github.com/arduino/ArduinoCore-avr/pull/383