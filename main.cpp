#include <iostream>
#include <xaudio2.h>
#include <combaseapi.h>
#include "AudioSettings.h"
#include "Oscillator.h"



int main() {
    using namespace Synth;

    // channels 1-2
    // sampleRate 22050-48000
    // resolution = 32
    // bufferSize 256-4096
    AudioSettings settings {2, 44100, 32, 1024};

    // Initialise COM
    if (HRESULT result = CoInitializeEx(NULL, COINIT_MULTITHREADED); result < 0) {
        return result;
    }
    // Create XAudio2 engine
    IXAudio2* xAudio2;
    if (HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR); result < 0) {
        return result;
    }
    // Create mastering voice
    IXAudio2MasteringVoice *masteringVoice;
    xAudio2->CreateMasteringVoice(&masteringVoice, settings.channels);

    // Create oscillator
    Oscillator* osc = new Oscillator(settings, xAudio2);


    char s;
    std::cout << "Enter any key to stop: ";
    std::cin >> s;
    xAudio2->Release();
    std::cout << "XAudio2 Released" << '\n';
    return 0;
}





