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
	 emu/gb/rom/mbc3.cpp \
	 emu/gb/audio/mixer.cpp \
	 emu/gb/audio/channel.cpp \
	 emu/gb/audio/square.cpp \
	 emu/gb/audio/noise.cpp \
	 emu/gb/audio/wave.cpp

LAUNCHER_SRC = launcher/launcher.cpp \
	       launcher/rom.cpp \
	       launcher/romlist.cpp \
	       launcher/toolbar.cpp

KEYREADER_SRC = keyreader/main.cpp \
		keyreader/keyreader.cpp \
		keyreader/config.cpp

MAIN_OBJ = $(addprefix obj/,$(MAIN_SRC:.cpp=.o))
GB_OBJ = $(addprefix obj/, $(GB_SRC:.cpp=.o))
LAUNCHER_OBJ = $(addprefix obj/, $(LAUNCHER_SRC:.cpp=.o))
KEYREADER_OBJ = $(addprefix obj/, $(KEYREADER_SRC:.cpp=.o))

CXX = g++
CXXFLAGS = -Wall -std=c++17 -g
SDLLIBS = $(shell sdl2-config --libs)
SDLFLAGS = $(shell sdl2-config --cflags)
WXFLAGS = $(shell wx-config --cxxflags)
WXLIBS = $(shell wx-config --libs)

all: gb launcher keyreader

.PHONY: clean
clean:
	rm -rf obj
	rm -f gb
	rm -f launcher
	rm -f keyreader

debug: CXXFLAGS += -DDEBUG
debug: gb launcher keyreader

gb: $(MAIN_OBJ) $(GB_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SDLLIBS)

launcher: $(LAUNCHER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(WXLIBS)
	mkdir -p roms
	mkdir -p save

keyreader: $(KEYREADER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SDLLIBS) -lSDL2_image

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(WXFLAGS) $(SDLFLAGS) -c $^ -o $@
