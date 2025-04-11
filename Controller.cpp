#include "Controller.h"
#include "UI/SynthUI.h"

int Controller::Start(int argc, char *argv[]) {
    // Create UI window
    QApplication app(argc, argv);
    window = new SynthUi(this);
    window->show();

    // Initialise COM
    if (HRESULT result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); result < 0) {
        return result;
    }
    // Create XAudio2 engine
    if (HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR); result < 0) {
        return result;
    }
    // Create mastering voice
    xAudio2->CreateMasteringVoice(&masteringVoice, settings.channels);
    masteringVoice->SetVolume(0.5f);

    // Create oscillators
    for (int i = 0; i < 8; i++) {
        oscillators.push_back(new Oscillator(i, this, settings, xAudio2, 220.0f));
        oscInUse[i] = false;
    }

    // Run UI logic
    int result = app.exec();

    // Cleanup
    xAudio2->Release();
    std::cout << "XAudio2 Released" << '\n';
    return result;
}

void Controller::PlayNote(char key) {
    float frequency = 220.0f * std::pow(2.0f, keySemitone[key] / 12.0f);
    for (int i = 0; i < 8; i++) {
        if (!oscInUse[i]) {
            oscInUse[i] = true;
            heldKeys[key] = i;
            oscillators[i]->Start(frequency);
            return;
        }
    }
}

void Controller::ReleaseNote(char key) {
    oscillators[heldKeys[key]]->Release();
    heldKeys.erase(key);
}

void Controller::CleanupOscillator(int id) {
    oscInUse[id] = false;
}

void Controller::RenderWaveform(BYTE* data) {
    window->SetAudioData(data);
}

void Controller::SetMasterVolume(int value) {
    masteringVoice->SetVolume((float)value / 100);
}

void Controller::SetWaveform(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetWaveform(value % 4, isA); }
}

void Controller::SetOscillatorVolume(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetVolume((float)value / 100, isA); }
}

void Controller::SetUnison(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetUnison(value, isA); }
}

void Controller::SetDetune(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetDetune(value, isA); }
}

void Controller::SetUnisonWidth(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetUnisonWidth(value, isA); }
}

void Controller::SetModulationType(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetModulationType(value, isA); }
}

void Controller::SetModulationValue(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetModulationValue((float)value / 100, isA); }
}

void Controller::SetOctaveSemitone(int value, bool isOctave, bool isA) {
    if (isOctave) {
        for (auto &entry : oscillators) { entry->SetOctave(value, isA); }
    } else {
        for (auto &entry : oscillators) { entry->SetSemitone(value, isA); }
    }
}

void Controller::SetPan(int value, bool isA) {
    for (auto &entry : oscillators) { entry->SetPan((float)value / 100, isA); }
}

float Controller::SetADSR(int value, char fieldId) {
    float fValue;
    switch (fieldId) {
        case 'A':
        case 'D':
        case 'R':
            fValue = Exponential(((float)value / 100), expCurve) * 8.0f; break;
        case 'S':
            fValue = (float)value / 100; break;
    }
    for (auto &entry : oscillators) { entry->SetADSR(fValue, fieldId); }
    return fValue;
}

void Controller::SetFilterSelection(bool value, bool isA) {
    for (auto &entry : oscillators) { entry->SetFilterSelection(value, isA); }
}

void Controller::SetFilterType(int value) {
    for (auto &entry : oscillators) { entry->SetFilterType(value); }
}

float Controller::SetFilterVariable(int value, char fieldId) {
    float fValue;
    switch (fieldId) {
        case 'F':
            fValue = std::clamp(Exponential(((float)value / 100), expCurve) * 22000.0f, 1.0f, 22000.0f); break;
        case 'R':
            fValue = std::clamp((float)value / 100, 0.01f, 1.0f); break;
    }
    for (auto &entry : oscillators) { entry->SetFilterVariable(fValue, fieldId); }
    return fValue;
}

float Controller::Exponential(float x, float curve) {
    return (curve * std::exp(x * (std::log((1 + curve) / curve))) - curve);
}