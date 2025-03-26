#ifndef SYNTH_OSCILLATOR_H
#define SYNTH_OSCILLATOR_H

#include <xaudio2.h>
#include <cmath>
#include <numbers>
#include "AudioSettings.h"
class VoiceCallback;
class Controller;

class Oscillator {
public:
    explicit Oscillator(Controller* controllerRef, AudioSettings settings, IXAudio2* xaudio2Interf);
    void AddBuffer(void* pBufferContext);
    void SetWaveform(int value);
    void SetVolume(float value);
private:
    int GenerateAudio(BYTE* pcmAudio, const DWORD* lengthInBytes, int phase);

    //enum Waveforms { Sine, Square, Triangle, Sawtooth };
    union Context {
        int id;
        void *ptr;
    };
    //AudioSettings audioSettings;
    Controller* controller;
    IXAudio2* xAudio2;
    const unsigned short channels;
    const unsigned long sampleRate;
    const unsigned short resolution;
    const int bufferSize;
    const DWORD bytesPerSec;
    const WORD blockAlign;
    const DWORD bufferSizeBytes;
    IXAudio2SubmixVoice* submixVoice = nullptr;
    IXAudio2SourceVoice* sourceVoice = nullptr;
    BYTE* pcmAudio1;
    BYTE* pcmAudio2;
    XAUDIO2_BUFFER buffer1 {0, bufferSizeBytes, pcmAudio1, 0, 0, 0, 0, 0, (void*)1};
    XAUDIO2_BUFFER buffer2 {0, bufferSizeBytes, pcmAudio2, 0, 0, 0, 0, 0, (void*)2};
    int waveform = 0;
    int currentPhase = 0;
};

#endif //SYNTH_OSCILLATOR_H
