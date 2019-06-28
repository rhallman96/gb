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

LAUNCHER_SRC = launcher/launcher.cpp \
	       launcher/rom.cpp \
	       launcher/romlist.cpp \
	       launcher/toolbar.cpp

MAIN_OBJ = $(addprefix obj/,$(MAIN_SRC:.cpp=.o))
GB_OBJ = $(addprefix obj/, $(GB_SRC:.cpp=.o))
LAUNCHER_OBJ = $(addprefix obj/, $(LAUNCHER_SRC:.cpp=.o))

CXX = g++
CXXFLAGS = -Wall -std=c++17 -g $(DEBUG_FLAG)
SDLLIBS = $(shell sdl2-config --libs)
SDLFLAGS = $(shell sdl2-config --cflags)
WXFLAGS = $(shell wx-config --cxxflags)
WXLIBS = $(shell wx-config --libs)

all: gb launcher

.PHONY: clean
clean:
	rm -rf obj
	rm -f gb
	rm -f launcher
	rm -rf save
	rm -rf roms 

.PHONY: debug
debug:
	make 'DEBUG_FLAG=-D DEBUG_MODE=true'

gb: $(MAIN_OBJ) $(GB_OBJ)
	$(CXX) $(CXXFLAGS) $(SDLLIBS) $^ -o $@

launcher: $(LAUNCHER_OBJ)
	$(CXX) $(CXXFLAGS) $(WXLIBS) $^ -o $@
	mkdir -p roms
	mkdir -p save

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(WXFLAGS) $(SDLFLAGS) -c $^ -o $@
