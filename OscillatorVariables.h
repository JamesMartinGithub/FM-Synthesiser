#ifndef SYNTH_OSCILLATORVARIABLES_H
#define SYNTH_OSCILLATORVARIABLES_H

struct OscillatorVariables {
int waveform[2] = {0};
float volume[2] = {0.5f, 0.0f};
int unison[2] = {1, 1};
int detune[2] = {0};
int unisonWidth[2] = {0};
int modType[2] = {0};
float modValue[2] = {0};
int octave[2] = {0};
int semitone[2] = {0};
float pan[2] = {0.5f, 0.5f};
};

#endif //SYNTH_OSCILLATORVARIABLES_H
