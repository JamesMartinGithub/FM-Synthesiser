#ifndef SYNTH_AUDIOSETTINGS_H
#define SYNTH_AUDIOSETTINGS_H

struct AudioSettings {
    unsigned short channels = 2;
    unsigned long sampleRate = 44100;
    unsigned short resolution = 32;
    int bufferSize = 1024;
};

#endif //SYNTH_AUDIOSETTINGS_H