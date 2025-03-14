# VL53L8CH via SPI using Mbed platform
**Objective:** To create a working VL53L8CH via SPI communication using Mbed platform, targetting the Blackpill Microcontroller (F411RE chipset). To accomplish this, VL53L8CH_LIB/L8_Platform must be modified to use SPI Mbed API.
The software will perform exactly like Arduino version, but built and code on Mbed.

Objective
1. Download Mbed Studio
2. Clone this repo
3. Create empty project in Mbed
4. Set up target to Blackpill_F411RE
5. Run Human_Read_able / Binary Mode
6. Hardware configuration

### 1. Download Mbed Studio
Use the following link to download mbed studio https://os.mbed.com/studio/
You are required to create arm mbed account in order to use mbed studio. Online version isn't recommended

### 2. Clone this repo
Clone this repo and place it somewhere you can access. This repo doesn't contain all the files to run the software right off the bat.
We will copy and paste the content of the folder into an mbed project.

### 3. Create empty project in Mbed
Navigate top right, File -> New Program
* Under Example Program select: **empty Mbed OS Program**
* Name your program
* Refer to this image [image](https://github.com/user-attachments/assets/793442d0-c3fd-4bab-9731-d7c174be2226)

### 4. Set up target to Blackpill_F411RE
* Right-click on the program's root folder and in the popup window select `Add library...`
* Copy&paste the text **https://os.mbed.com/users/hudakz/code/BLACKPILL_Custom_Target** into the `Git or os.mbed.com URL` edit box and click on the `Next` button.
* Open the drop-list and select `default` as `Branch or tag` and click on the `Finish` button.
* Open the `BLACKPILL_Custom_Target` folder and according to your board drag&drop
  the `TARGET_BLACKPILL_F401CC` or the `TARGET_BLACKPILL_F411CE` folder and
  the `custom_targets.json` file one by one to the root folder of your program.
* Delete the `BLACKPILL_Custom_Target` folder from your project. (Right-click and select delete).
* Open the `Target` drop-list and click on the button with a "chip" icon on it (Manage custom targets).
* Open the `USB device` drop-list and select your STM32 ST-Link programmer.
* Open the Build target drop-list and according to your board select `BLACKPILL_F411CE`.

### 5. Run Human_Read_able / Binary Mode
Drag&Drop the remaining folders of the `cloned repo` one by one to the root folder of your program, including
* VL53L8CH_LIB
* src
* inc
* mbed_app.json


Remove initial main.cpp root folder


Click the hammer build button to create .BIN file

### 6. Flash the program to the board
* Download&Install the STM32Programmer [link](https://www.st.com/en/development-tools/stm32cubeprog.html)
![image](https://github.com/user-attachments/assets/dbdcbeb3-6fb1-4d9c-9667-fab08bfbe433)
* Select `ST-Link` as the connection mode & click `connect`
* click on `Open File` tab and choose .BIN file that you compile in step 5. (`C:\Users\Pittawat\Documents\UW-Madison Graphic Lab\Programming\SPI_VL53L8CH\BUILD\BLACKPILL_F411CE\ARMC6`)
![image](https://github.com/user-attachments/assets/571d23fb-ae16-4530-8110-ead4d70179c3)
* Click `download` to flash the board
* **To read from the sensor, ensure that the MCU is disconnected from the STM32Programmer. This can be done simply by unplugging and replugging the USB. The blue LED on the MCU should be on, and the ST-Link should not be flashing.**

### Debug Ideas
1. While testing the system, we notice that the FTDI (UART <-> USB device) doesn't get sufficient power, resulting in a dimmed LED light.
   if you notice that the LED isn't fully bright, unplug the ST-Link or FTDI to reset the system.

