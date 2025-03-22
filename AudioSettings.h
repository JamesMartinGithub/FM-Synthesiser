#ifndef SYNTH_AUDIOSETTINGS_H
#define SYNTH_AUDIOSETTINGS_H

namespace Synth {
    struct AudioSettings {
        unsigned short channels = 2;
        unsigned long sampleRate = 44100;
        unsigned short resolution = 32;
        int bufferSize = 1024;
    };
} // Synth

#endif //SYNTH_AUDIOSETTINGS_H