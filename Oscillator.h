#ifndef SYNTH_OSCILLATOR_H
#define SYNTH_OSCILLATOR_H

#include <xaudio2.h>
#include <cmath>
#include <numbers>
#include "AudioSettings.h"
#include "RandomGenerator.h"
class VoiceCallback;
class Controller;

class Oscillator {
public:
    explicit Oscillator(Controller* controllerRef, AudioSettings settings, IXAudio2* xaudio2Interf);
    void AddBuffer(void* pBufferContext);
    void SetWaveform(int pId, bool isA);
    void SetVolume(float pVolume, bool isA);
    void SetUnison(int pVoiceCount, bool isA);
    void SetDetune(int pDetunePercent, bool isA);
    void SetUnisonWidth(int pWidthPercent, bool isA);
    void SetModulationType(int pModulationType, bool isA);
    void SetModulationValue(float pModulationValue, bool isA);
    void SetOctave(int pOctave, bool isA);
    void SetSemitone(int pSemitone, bool isA);
private:
    void GenerateAudio(BYTE* pcmAudio, const DWORD* lengthInBytes);

    union Context {
        int id;
        void *ptr;
    };
    union Pcm {
        BYTE* bytes;
        float* floats;
    };
    Controller* controller;
    IXAudio2* xAudio2;
    const unsigned short channels;
    const unsigned long sampleRate;
    const unsigned short resolution;
    const int bufferSize;
    const DWORD bytesPerSec;
    const WORD blockAlign;
    const DWORD bufferSizeBytes;
    const int bufferExtraSamples = 0;
    const float maxDetune = 20.0f;
    const double wavelength = 2 * std::numbers::pi;
    const double halfWavelength = std::numbers::pi;
    IXAudio2SubmixVoice* submixVoice = nullptr;
    IXAudio2SourceVoice* sourceVoice = nullptr;
    BYTE* pcmAudio1;
    BYTE* pcmAudio2;
    union Pcm pcm;
    XAUDIO2_BUFFER buffer1 {0, bufferSizeBytes, pcmAudio1, 0, 0, 0, 0, 0, (void*)1};
    XAUDIO2_BUFFER buffer2 {0, bufferSizeBytes, pcmAudio2, 0, 0, 0, 0, 0, (void*)2};
    double phase[18] = {0};
    int waveform[2] = {0};
    float amplitude[2] = {0.0f, 0.5f};
    float baseFrequency = 220.0f;
    int voiceCount[2] = {1, 1};
    float detune[2] = {0};
    float unisonWidth[2] = {0};
    float lastSampleL[2] = {0};
    float lastSampleR[2] = {0};
    int modulatorOsc = 0;
    int carrierOsc = 1;
    int modulationType[2] = {0};
    float modulationAmount[2] = {0};
    int octave[2] = {0};
    int semitone[2] = {0};
    float oscFrequency[2] = {baseFrequency, baseFrequency};
};

#endif //SYNTH_OSCILLATOR_H
