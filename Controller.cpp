#include "Controller.h"
#include "UI/SynthUI.h"

int Controller::Start(int argc, char *argv[]) {
    // Create UI window
    QApplication app(argc, argv);
    window = new SynthUi(this);
    window->show();

    // channels 1-2
    // sampleRate 22050-48000
    // resolution = 32
    // bufferSize 256-4096
    AudioSettings settings {2, 44100, 32, 1024};

    // Initialise COM
    if (HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); result < 0) {
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
    oscA = new Oscillator(this, settings, xAudio2);

    // Run UI logic
    int result = app.exec();

    // Cleanup
    xAudio2->Release();
    std::cout << "XAudio2 Released" << '\n';
    return result;
}

void Controller::RenderWaveform(BYTE* data) {
    window->SetAudioDataA(data);
}

void Controller::on_waveformDialA_valueChanged(int value) {
    oscA->SetWaveform(value % 4);
}

void Controller::on_volumeFaderA_valueChanged(int value) {
    oscA->SetVolume((float)value / 100);
}