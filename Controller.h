#ifndef SYNTH_CONTROLLER_H
#define SYNTH_CONTROLLER_H

#include <iostream>
#include <string>
#include <xaudio2.h>
#include <combaseapi.h>
#include "AudioSettings.h"
#include "Oscillator.h"
#include <QApplication>

class SynthUi;

class Controller {
public:
    Controller(){}
    int Start(int argc, char *argv[]);
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
private:
    SynthUi* window;
    IXAudio2MasteringVoice *masteringVoice;
    Oscillator* osc;
};

#endif //SYNTH_CONTROLLER_H
