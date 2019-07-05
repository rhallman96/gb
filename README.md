# game boy
This is a Nintendo Game Boy emulator implemented in C++ using SDL for graphics and keyboard input. This program is designed for Mac OS and Linux, and is not compatible with Windows.

## Dependencies
+ SDL2 / SDL2_Image
+ wxWidgets 3.0 or higher (for the launcher)

## Build
In order to build this program, simply cd into the main project folder and type `make`. The preferred way to launch the emulator is through the graphical launcher, which can be either double clicked or run from the terminal without any additional command line arguments.

Alternatively, the executable file will be named `gb`, and can be run like so:

`./gb [rom name] [save file]`

In order to build the program in debug mode, type `make debug`.

## Controls

You can configure custom key mappings by clicking the controller button in the launcher. However, the default key mappings are:
+ **Arrow Keys** : D Pad
+ **X** : A Button
+ **Z** : B Button
+ **Enter** : Start
+ **Right Shift** : Select

## Debug Mode
Debug mode allows you to step through the code on a per-instruction basis, set breakpoints, and dump the values of the system's registers and specific addresses.

+ **r** : run (until the next breakpoint is reached)
+ **s** : step to the next instruction
+ **i** : dump the current register values
+ **i** [addr] : dump the value at addr
+ **b** [addr] : set the breakpoint at addr
