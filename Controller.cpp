#include "Controller.h"
#include "UI/SynthUI.h"

int Controller::Start(int argc, char *argv[]) {
    // Create UI window
    QApplication app(argc, argv);
    window = new SynthUi(this);
    window->show();

    // Supportable settings:
    //     channels 1-2
    //     sampleRate 22050-48000
    //     resolution = 32
    //     bufferSize 256-4096
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
    xAudio2->CreateMasteringVoice(&masteringVoice, settings.channels);
    masteringVoice->SetVolume(0.5f);

    // Create oscillator
    osc = new Oscillator(this, settings, xAudio2);

    // Run UI logic
    int result = app.exec();

    // Cleanup
    xAudio2->Release();
    std::cout << "XAudio2 Released" << '\n';
    return result;
}

void Controller::RenderWaveform(BYTE* data) {
    window->SetAudioData(data);
}

void Controller::SetMasterVolume(int value) {
    masteringVoice->SetVolume((float)value / 100);
}

void Controller::SetWaveform(int value, bool isA) {
    osc->SetWaveform(value % 4, isA);
}

void Controller::SetOscillatorVolume(int value, bool isA) {
    osc->SetVolume((float)value / 100, isA);
}

void Controller::SetUnison(int value, bool isA) {
    osc->SetUnison(value, isA);
}

void Controller::SetDetune(int value, bool isA) {
    osc->SetDetune(value, isA);
}

void Controller::SetUnisonWidth(int value, bool isA) {
    osc->SetUnisonWidth(value, isA);
}

void Controller::SetModulationType(int value, bool isA) {
    osc->SetModulationType(value, isA);
}

void Controller::SetModulationValue(int value, bool isA) {
    osc->SetModulationValue((float)value / 100, isA);
}

void Controller::SetOctaveSemitone(int value, bool isOctave, bool isA) {
    if (isOctave) {
        osc->SetOctave(value, isA);
    } else {
        osc->SetSemitone(value, isA);
    }
}