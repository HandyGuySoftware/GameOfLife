Game of Life Simulator ReadMe
-----------------------------

The Game of Life simulator simulates Conway's Game of Lie on a 16x32 LED panel. Much has been written about Conway's Game, so no need to rehash all that here. For starters, check out https://en.wikipedia.org/wiki/Conway's_Game_of_Life.

To start the Game just plug in the power supply. The game will initialize and start by itself.

Configuring the Game
--------------------
The Game hardware uses two buttons: Mode and Set. Pressing the Mode button will put the clock into Setup Mode. The LED on the Set button will light up, the display will blank, and you will see the first of the configuration screens. To move through the options press the Setup button. The options screens are:

Randomize Pixels: Select whether or not to set each pixel on the board to a random color. If set to "Yes" game play will slow down a bit due to all the random number generation (but it looks very cool!). Default=No.

Random Game Color: All the pixels in a game will be the same color, but each new game will select a new random color for the pixels. If both Random Pixels and Random Game are set to "Yes", Random Pixels takes precedence. Default=Yes.

Game Delay: Amount of delay between generations in a game. Values are 0-5, representing multiples of 200ms. Default=0;

Timeout: Number of endless loop cycles before the game is declared over. More on this below. Default=200.

Wrap: Set whether pixels will wrap around the edges of the screen. Default=No.

Show Generations: Set whether or not to show how many generations were created at the end of the game. Default=No.

Reset: Reset game parameters to all defaults.

Endless Loop Prevention
-----------------------
The Game of Life code provides the opportunity for many forms of endless loops, causing the game to go on forever. If you want to display the game on a shelf (like I did) and not have to worry about resetting it periodically, endless loop protection is built into the code through pattern tracking, pixel counting, etc. You can adjust how many rounds of loops are executed before resetting the game as part of the game options. If you want to allow endless loops as part of the game, you will have to edit the code.

Parts
-----
These are the major parts used. Incidental parts (terminal blocks, power jacks, connecting cables, etc.) are left to the imagination of the builder.

1 - Arduino Uno
1 - 16x32 LED Matrix (https://www.adafruit.com/products/420)
1 - Adafruit ProtoShield (http://www.adafruit.com/products/2077)
1 - 16mm Illuminated Pushbutton (Mode Button)([http://www.adafruit.com/products/1479)
1 - 16mm Panel Mount Momentary Pushbutton -  Black (Set button) (http://www.adafruit.com/products/1505)
1 - 5V 4A (4000mA) switching power supply - (http://www.adafruit.com/products/1466)
1 - Project Box (7.5" x 4.3" x 2.2") (191mm x 110mm x 57mm) - http://www.amazon.com/gp/product/B0002BSRIO?psc=1&redirect=true&ref_=oh_aui_detailpage_o02_s00
1 - 330 Ohm Resistor
2 - 10kOhm Resistors
1 - 4700 uf Capacitor

Arduino/C++ Code Modules
------------------------
Life1.ino - The main game code. Includes setup() and loop() routines.

gameBoard.h/gameBoard.cpp - Class to manage all aspects of the game board logic and display.

hardware_Adafruit_LED_16x32.h/hardware_Adafruit_LED_16x32.cpp - Class to manage the specific LED display used in the project. The original idea was to make the game hardware independent and just replace the hardware_* modules for new display boards. That never happened (see Memory Usage below).

Code Complexity
---------------
The basic game logic only takes maybe 50 lines of code. The rest of this monstrosity is taken up by option management and display cuteness. This code is WAAAAY more complicated than it needs to be, but it was an experiment/learning project, so why not.

Memory Usage
------------
As described above, the original goal of the project was to create a game that would be display hardware independent. That is, I would have liked to be able to swap the 16x32 display out for another type of board (for example, a 800x480 TFT display), rewrite the hardware module (hardware_*_.cpp) to account for writing to the new hardware, and off you go. Unfortunately, I immediately began to hit memory limits in the Arduino Uno. The larger display requires tracking many (many, many) more pixels, and the 32k memory limit of the Uno just couldn't handle it. Yes, I could have gone with a bigger processor with more memory, but even the Mega's 256k started to strain under the memory load as the displays got larger. I researched various implementations of sparse matrices for about 6 weeks before giving up the whole idea and moving on to other shiny objects. Maybe someday I'll get back to it.

Miscellaneous
-------------

Disclaimer: I used to be a professional programmer, but that was back in the stone age when we used to chisel our code into stone tablets. I have probably violated numerous current C/C++/Arduino coding conventions in the source code. Sorry. Not sorry. The game works, so 'nuff said ;-) If someone wants to politely point out where I can bring the code up to current standards I will be happy to do so in a future update.

Release Notes
-------------
1.0 - Initial release. Have at it. Please let me know what you think.

