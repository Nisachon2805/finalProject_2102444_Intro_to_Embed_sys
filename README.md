# finalProject_2102444_Intro_to_Embed_sys
## Emergency System with traffic light
 This Arduino project controls traffic lights and a sound alarm based on sensor input. It utilizes various components such as an LCD display, keypad, RGB LEDs, and a buzzer to create an emergency notification system.
 (It will need to be addressed in the future due to limitations in terms of device numbers and other factors, making this project incomplete.)
## Components Used
+ Arduino board
+ I2C 16x2 LCD Display
+ Matrix Keypad Module
+ 4 digit 7 segment
+ Microphone Sensor
+ RGB LED (for one road)
+ Buzzer 
+ Tact Switch
+ LED 5mm (for another road)

## Libraries Used
+ Wire.h
+ LiquidCrystal_I2C.h
+ Keypad.h
+ PinChangeInterrupt.h
+ SevSeg.h

## Functionality
The system operates in two modes: normal and emergency.

### Normal Mode
Monitors the sound level using the sound sensor.
Activates the buzzer and displays an emergency message on the LCD if the sound level exceeds a predefined threshold.
Controls the LED for red ang green.
### Emergency Mode
Triggered by pressing a button.
Allows the user to set the duration of the emergency (1 or 2 second) using the keypad.
Controls the RGB LED in the emergency mode.

## Software
### lightControl(String Mode)
The lightControl function is responsible for controlling the lighting system based on the specified mode.
#### Parameters
Mode: Specifies the mode of operation, either "normal" or "emergency".
#### Normal Mode("normal")
In normal mode, the function controlling the lighting system on normal operation by toggling between green and red lights. It also handles the countdown timer.
#### Emergency Mode("emerg")
In emergency mode, the function coordinates the lighting system on the emergency status. It manages the countdown timer for the emergency duration and traffig lights.
- call handleEmerg() to interaction with admin

### handleEmerg()
The handleEmerg function is responsible for managing the emergency handling process. Initiates the countdown timer for the emergency duration.
#### State
- If the system is in state 0, it prompts the user to enter the emergency duration using a keypad. Upon input, it transitions to state 1.
- If the system is in state 1, it validates the entered emergency duration and updates the state accordingly. Once the countdown timer reaches 0, it transitions to state 2, indicating the completion of the emergency handling process.
- If the system is in state 2, it completes the emergency handling process and waits for a new emergency to occur.

### countingTime(String Mode)
The countingTime function manages the countdown timer for the traffig light. It counts down the time in milliseconds and updates the 7-segment display accordingly.

### Emerg()
The Emerg function toggles the emergency state of the system when has interruppt from button.

### oneLight(String Mode) and twoLight(String Mode)
These function set hoe to out put LED or RGB LED to show red green or yellow ligth.
