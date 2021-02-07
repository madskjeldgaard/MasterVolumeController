![In a box made for screws](assets/screws1.jpg)

## Compiling
The project is setup as platformio project.

From the root of this repo run:
```bash
pio run -t upload
```

This will compile and upload the code to your Teensy

# Usage
Turning the controller on, it will work in normal 7 bit midi mode.

If you are pressing the button while it is booting, it will go into 14 bit midi mode (marked by blinking lights). The midi cc num for the pot is now 1 higher than before. See the modality example in the SuperCollider folder for an example.

## Connections

Button:
- Connect pin 1 of button to 3.3v
- Connect pin 2 of button to pin 1

Pot:
- Connect pin1 to 3.3v
- Connect pin2 to A0 (pin 14)
- Connect pin3 to GND

## Using as Server volume controller in SuperCollider

Setting this up to use as master volume controller in SuperCollider is pretty easy. 
Copy/paste the code in the supercollider example file into your SuperCollider startup file to automatically set this up.

![In a box made for screws](assets/screws2.jpg)
