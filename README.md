# game boy
This is a Nintendo Game Boy emulator implemented in C++ using SDL for graphics and keyboard input. This program is currently only compatible with Linux and Mac OSX.

## Dependencies
+ SDL2
+ wxWidgets 3.0 or higher (for the launcher)

## Build
In order to build this program, simply cd into the main project folder and type `make`. The executable file will be named `gb`, and can be run like so:

`./gb [rom name] [save file]`

In order to build the program in debug mode, type `make debug`.

## Controls
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
