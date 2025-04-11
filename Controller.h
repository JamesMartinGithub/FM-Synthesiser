#ifndef SYNTH_CONTROLLER_H
#define SYNTH_CONTROLLER_H

#include <iostream>
#include <string>
#include <deque>
#include <xaudio2.h>
#include <combaseapi.h>
#include "AudioSettings.h"
#include "ADSR.h"
#include "Oscillator.h"
#include <QApplication>

class SynthUi;

class Controller {
public:
    Controller(){}
    int Start(int argc, char *argv[]);
    void PlayNote(char key);
    void ReleaseNote(char key);
    void CleanupOscillator(int id);
    void RenderWaveform(BYTE* data);
    void SetMasterVolume(int value);
    void SetWaveform(int value, bool isA);
    void SetOscillatorVolume(int value, bool isA);
    void SetUnison(int value, bool isA);
    void SetDetune(int value, bool isA);
    void SetUnisonWidth(int value, bool isA);
    void SetModulationType(int value, bool isA);
    void SetModulationValue(int value, bool isA);
    void SetOctaveSemitone(int value, bool isOctave, bool isA);
    void SetPan(int value, bool isA);
    float SetADSR(int value, char fieldId);
    void SetFilterSelection(bool value, bool isA);
    void SetFilterType(int value);
    float SetFilterVariable(int value, char fieldId);
private:
    float Exponential(float x, float curve);

    SynthUi* window;
    IXAudio2* xAudio2;
    IXAudio2MasteringVoice *masteringVoice;
    // Supportable settings:
    //     channels 1-2
    //     sampleRate 22050-48000
    //     resolution = 32
    //     bufferSize 256-4096
    AudioSettings settings {2, 44100, 32, 1024};
    std::vector<Oscillator*> oscillators;
    bool oscInUse[8];
    std::map<char, int> heldKeys;
    std::map<char, float> keySemitone = {{'Z', 0.0f}, {'S', 1.0f}, {'X', 2.0f}, {'D', 3.0f}, {'C', 4.0f}, {'V', 5.0f}, {'G', 6.0f}, {'B', 7.0f}, {'H', 8.0f}, {'N', 9.0f}, {'J', 10.0f}, {'M', 11.0f}, {',', 12.0f}, {'L', 13.0f}, {'.', 14.0f}, {';', 15.0f}, {'/', 16.0f}};
    float expCurve = 0.006f;
};

#endif //SYNTH_CONTROLLER_H
