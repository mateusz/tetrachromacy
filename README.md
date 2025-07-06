# Tetrachromacy tester

This project implements a DIY device for screening orange-cone tetrachromacy using Heterochromatic Flicker Photometry (HFP).

## Theory

The test rapidly alternates two light sources — one pure orange LED and one mixed red+green source — at ~20 Hz. The subject adjusts the red and green intensities using potentiometers to try and eliminate visible flicker. A successful flicker cancellation suggests typical trichromatic vision, while persistent flicker may indicate tetrachromatic perception.

## Build

The device is built using an Arduino Uno, three LEDs, three potentiometers, and simple materials like cardboard. Red and orange PWM channels are configured for high-resolution brightness control, while green PWM channel in low-res mode due to Arduino Uno limitations.

Three buttons are also used for control of the software functions, such as HFP frequency. A "run test" mode is used for the actual test, which just disables the screen refresh (which interferes with the test by introducing a blink every second or so).

![Schematics](schematics.jpg)

Here is the finished project:

![Electronics box](entire_setup.jpeg)

Cardboard diffusion box makes sure lights are thoroughly mixed by facing the LEDs away from the viewer and by using three layers of tracing paper.

![Diffusion box](diffusion_box.jpeg)

## Test run

This has been captured in low-res, low-frequency mode, but suitably demonstrates the general idea - it's hard to show the exact operation due to limitations of the camera and the video format.

Importantly, you can't use a video of this device for tetrachromacy testing. This is because neither the camera nor the file format is able carry orange light information. The screen can't produce pure orange either because it only has RGB LEDs - and pure orange is critical for this.

[Youtube video](https://www.youtube.com/watch?v=cta9pufByBE)

## Shopping list

Just a note to my future self, really.

| Quantity | Part Number    | Manufacturer Part Number | Description                                               | Customer Reference              |
| -------- | -------------- | ------------------------ | --------------------------------------------------------- | ------------------------------- |
| 4        | TLCPG5100-ND   | TLCPG5100                | Digikey - LED GREEN CLEAR T-1 3/4 T/H                     | 562 [563nm] 1250mcd 50mA 2.1V   |
| 4        | 754-1611-ND    | WP710A10SYCK             | Digikey - LED YELLOW CLEAR T-1 T/H                        | 590nm 1500mcd 20mA 2V           |
| 2        | 754-1894-ND    | WP710A10SRC/J4           | Digikey - Red 640nm LED Indication - Discrete 2.1V Radial | 640nm [660nm] 1500mcd 20mA 2.1V |
| 1        | 1528-1743-ND   | 1311                     | Digikey - HOOK-UP 22AWG SOLID - 6 X 25FT                  | good for 300 mA (0.3 A) @ 5V    |
| 3        | SBBTH1506-1-ND | SBBTH1506-1              | Digikey - BREADBOARD GENERAL PURPOSE PTH                  | small 15x8                      |
| 1        | 1528-4786-ND   | 4786                     | Digikey - BREADBOARD GENERAL PURPOSE PTH                  | large 18x22 (maybe 3?)          |
| 1        | SAM1112-32-ND  | TS-132-T-A               | Digikey - CONN HEADER VERT 32POS 2.54MM                   |                                 |
| 2        | 1568-1513-ND   | PRT-12796                | Digikey - JUMPER WIRE F/F 6" 20PCS                        | good for 50 mA @ 5V             |
| 2        | 1528-4448-ND   | 4448                     | CBL ASSY USB M-CB W/JUMPER 0.98'                          |                                 |
|          |                |                          | Leftover LCD screen 16x2                                  |                                 |
|          |                |                          | Leftover Geekcreeit "Arduino Uno"                         |                                 |
|          |                |                          | Leftover prototype shield                                 |                                 |
|          |                |                          | HD44780 LCD screen I2C backpack                           |                                 |
|          |                |                          | A box of pots from Aliexpress (B5K/10K etc)               |                                 |
|          |                |                          | Leftover buttons                                          |                                 |
|          |                |                          | Leftover resistors                                        |                                 |
|          |                |                          | A box of transistors from Aliexpress (2N2222 etc)         |                                 |
|          |                |                          | Leftover audio cable                                      |                                 |
|          |                |                          | Cardboard, tracing paper, hot glue, paper glue            |                                 |
|          |                |                          | A lunchbox                                                |                                 |
|          |                |                          | Leftover 3A USB power supply 5V                           |                                 |