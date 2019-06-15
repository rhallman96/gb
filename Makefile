MAIN_SRC = main.cpp \
	   window.cpp

GB_SRC = gb/z80.cpp \
	 gb/bus.cpp \
	 gb/rom/rom.cpp \
	 gb/gb.cpp \
	 gb/lcd.cpp \
	 gb/debug.cpp \
	 gb/devices.cpp \
	 gb/joypad.cpp \
	 gb/rom/mbc1.cpp 

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
