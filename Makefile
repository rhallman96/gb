MAIN_SRC = emu/main.cpp \
	   emu/window.cpp

GB_SRC = emu/gb/z80.cpp \
	 emu/gb/bus.cpp \
	 emu/gb/rom/rom.cpp \
	 emu/gb/gb.cpp \
	 emu/gb/lcd.cpp \
	 emu/gb/debug.cpp \
	 emu/gb/devices.cpp \
	 emu/gb/joypad.cpp \
	 emu/gb/rom/mbc1.cpp \
	 emu/gb/rom/mbc3.cpp

MAIN_OBJ = $(addprefix obj/,$(MAIN_SRC:.cpp=.o))
GB_OBJ = $(addprefix obj/, $(GB_SRC:.cpp=.o))

DEBUG_MODE=false

CXX = g++
CXXFLAGS = -Wall -std=c++11 -g -D DEBUG_MODE=$(DEBUG_MODE)
SDLFLAGS = $(shell sdl2-config --libs --cflags)

all: gb

.PHONY: clean
clean:
	rm -rf obj
	rm -f gb

.PHONY: debug
debug:
	make 'DEBUG_MODE=true'

gb: $(MAIN_OBJ) $(GB_OBJ)
	$(CXX) $(CXXFLAGS) $(DEBUG_OFF) $(SDLFLAGS) $^ -o $@


obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@
