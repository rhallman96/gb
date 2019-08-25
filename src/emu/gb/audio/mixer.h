#pragma once

#include "../bus.h"
#include "../devices.h"
#include "square.h"
#include "noise.h"
#include "wave.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

/**
 * @author Rick Hallman
 * This class handles audio playback and mixing for the emulator.
 */
class Mixer : public GenericDevice
{
public:

    static const int c_sampleRate = 44100;
    static const int c_bufferSize = 768;
    
    static const uint16_t c_start = 0xFF10;
    static const uint16_t c_end   = 0xFF3F;
    
    Mixer( void );  
    virtual ~Mixer( void );

    void access( uint16_t addr, uint8_t& data, bool write );
    
    /**
     * Update the audio mixer.
     * @param ticks the number of CPU ticks taken by the latest instruction
     */
    void update( uint8_t ticks );

private:

    /**
     * Indicates whether or not previous frame's sound is still playing.
     * @return true if playing, false otherwise
     */
    bool isSoundPlaying( void );
    
    /**
     * Opens a new SDL Audio device.
     * @return the device's ID
     */
    SDL_AudioDeviceID openDevice( void );

    /**
     * Gathers audio samples from the four audio channels 
     * and adds them to the SDL audio queue. The program is delayed
     * if samples from the previous frame are still playing 
     */
    void queueSamplesAndDelay( void );

    /**
     * Gets the volume for the SO2 output channel.
     * @return the volume (0-7)
     */
    int getLeftVolume( void );

    /**
     * Gets the volume for the SO1 output channel.
     * @return the volume (0-7)
     */
    int getRightVolume( void );

    /**
     * Indicates whether or not a sound is playing to the left output.
     * @param channel the channel number (1-4)
     * @return true if playing, false otherwise
     */
    bool isPlayingLeft( int channel );

    /**
     * Indicates whether or not a sound is playing to the right output.
     * @param channel the channel number (1-4)
     * @return true if playing, false otherwise
     */
    bool isPlayingRight( int channel );

    /**
     * Whether or not sound is enabled in general for the audio unit.
     * @return true if the sound is enabled, false otherwise
     */
    bool isSoundEnabled( void );
    
    /**
     * Clear out the contents of all sound registers.
     * Called when sound registers are disabled in 0xFF26.
     */
    void clearSoundRegisters( void );
    
    // audio channels
    Square* mp_square1;
    Square* mp_square2;
    Noise* mp_noise;
    Wave* mp_wave;
    
    // SDL audio device
    SDL_AudioDeviceID m_audioDevice;
    
    // left and right output volumes
    uint8_t m_lVolume, m_rVolume;
    
    // number of ticks counted
    long m_ticks;

    // the audio buffer
    Sint16* m_buffer;
    int m_bufferIndex;

    // OR values for READ access
    static const uint8_t c_orValues[23];
};
