# Autokey

## Upload Leonardo bootloader

### 1. Connect USBasp to Arduino using ICSP pins

<img src="images\usbasp.png" alt="usbasp" style="width:600px;"/>

### 2. Connect USBasp to PC

### 3. Install drivers USBasp (if required)

#### 3.1. Download and install Zadig: https://zadig.akeo.ie

#### 3.2. Select USBasp device, select target driver: WinUSB driver and Install Driver

![image](images\zadig.png)

#### 3.3. You can check if the driver has been installed correctly in Device Manager

### 4. Open Arduino IDE and select board: Arduino Leonardo

-> Tools -> Board -> Arduino AVR Boards -> Arduino Leonardo

![image](images\arduinoide-leonardo.png)

### 5. Select Programmer: USBasp

-> Tools -> Programmer -> USBasp

![image](images\arduinoide-usbasp.png)

### 5. Start burning the bootloader

-> Tools -> Burn Bootloader

![image](images\arduinoide-bootloader.png)

### 6. Done!

![image](images\arduinoide-done.png)

### References: 
- https://www.instructables.com/Fixing-Bootloader-on-Arduino-Leonardo-ProMicro-Wit/
- https://www.fischl.de/usbasp/
- https://electronics.stackexchange.com/questions/416714/avrdude-does-not-recognize-usbasp-device/417509#417509
