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
    void on_waveformDialA_valueChanged(int value);
    void on_volumeFaderA_valueChanged(int value);
private:
    SynthUi* window;
    Oscillator* oscA;
};

#endif //SYNTH_CONTROLLER_H
