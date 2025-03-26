#include "Oscillator.h"
#include "VoiceCallback.h"
#include "Controller.h"

Oscillator::Oscillator(Controller* controllerRef, AudioSettings settings, IXAudio2* xaudio2Interf)
    : controller(controllerRef)
    , channels(settings.channels)
    , sampleRate(settings.sampleRate)
    , resolution(settings.resolution)
    , bufferSize(settings.bufferSize)
    , bytesPerSec((sampleRate * channels * resolution) / 8)
    , blockAlign((channels * resolution) / 8)
    , bufferSizeBytes(bufferSize * 8)
    , xAudio2(xaudio2Interf) {
    // Create format object
    const WAVEFORMATEX format{WAVE_FORMAT_IEEE_FLOAT, channels, sampleRate, bytesPerSec, blockAlign, resolution, 0};

    // Generate initial audio for both buffers
    pcmAudio1 = new BYTE[bufferSizeBytes];
    pcmAudio2 = new BYTE[bufferSizeBytes];
    buffer1.pAudioData = pcmAudio1;
    buffer2.pAudioData = pcmAudio2;
    currentPhase = GenerateAudio(pcmAudio1, &bufferSizeBytes, 0);
    currentPhase = GenerateAudio(pcmAudio2, &bufferSizeBytes, currentPhase);

    // Create submix voice
    xAudio2->CreateSubmixVoice(&submixVoice, channels, sampleRate, 0, 0);
    submixVoice->SetVolume(0.5f);
    XAUDIO2_SEND_DESCRIPTOR sendDescriptor = {0, submixVoice};
    XAUDIO2_VOICE_SENDS sendList = {1, &sendDescriptor};

    // Create source voice and submit both buffers
    VoiceCallback* voiceCallback = new VoiceCallback(&Oscillator::AddBuffer, this);
    xAudio2->CreateSourceVoice(&sourceVoice, &format, 0, 2.0f, voiceCallback, &sendList);
    sourceVoice->SubmitSourceBuffer(&buffer1);
    sourceVoice->SubmitSourceBuffer(&buffer2);
    // Start voice audio
    sourceVoice->Start(0);
}

int Oscillator::GenerateAudio(BYTE *pcmAudio, const DWORD *lengthInBytes, int phase) {
    float t;
    float decValue;
    float a = 0.4f;
    float frequency = 220.0f;
    switch (waveform) {
        case 0: {
            float w = 2 * std::numbers::pi * frequency;
            for (int i = 0; i <= *lengthInBytes - 8; i += 8) {
                t = (float) (i / 8 + phase) / sampleRate;
                decValue = a * sin(w * (float) t);
                memcpy(&pcmAudio[i], &decValue, 4);
                memcpy(&pcmAudio[i + 4], &decValue, 4);
            }
            break;
        }
        case 1: {
            float wavelength = sampleRate / frequency;
            float halfWavelength = wavelength / 2.0f;
            for (int i = 0; i <= *lengthInBytes - 8; i += 8) {
                t = fmod(((float) i / 8) + phase, wavelength);
                decValue = t > halfWavelength ? a : 0;
                memcpy(&pcmAudio[i], &decValue, 4);
                memcpy(&pcmAudio[i + 4], &decValue, 4);
            }
            break;
        }
        case 2: {
            float wavelength = sampleRate / frequency;
            float halfWavelength = wavelength / 2.0f;
            for (int i = 0; i <= *lengthInBytes - 8; i += 8) {
                t = fmod(((float) i / 8) + phase, wavelength);
                decValue = a * (abs((t / halfWavelength) - 1) - 0.5f) * 2;
                memcpy(&pcmAudio[i], &decValue, 4);
                memcpy(&pcmAudio[i + 4], &decValue, 4);
            }
            break;
        }
        case 3: {
            float wavelength = sampleRate / frequency;
            float halfWavelength = wavelength / 2.0f;
            for (int i = 0; i <= *lengthInBytes - 8; i += 8) {
                t = fmod(((float) i / 8) + phase, wavelength);
                decValue = a * ((t / wavelength) - 0.5f);
                memcpy(&pcmAudio[i], &decValue, 4);
                memcpy(&pcmAudio[i + 4], &decValue, 4);
            }
            break;
        }
    }
    return fmod(((*lengthInBytes) / 8) + phase, sampleRate / frequency);
}

// populates and adds the buffer that just ended
void Oscillator::AddBuffer(void *pBufferContext) {
    Context context;
    context.ptr = pBufferContext;
    if (context.id == 1) {
        currentPhase = GenerateAudio(pcmAudio1, &bufferSizeBytes, currentPhase);
        sourceVoice->SubmitSourceBuffer(&buffer1);
        controller->RenderWaveform(pcmAudio1);
    } else {
        currentPhase = GenerateAudio(pcmAudio2, &bufferSizeBytes, currentPhase);
        sourceVoice->SubmitSourceBuffer(&buffer2);
    }
}

void Oscillator::SetWaveform(int value) {
    waveform = value;
}

void Oscillator::SetVolume(float value) {
    submixVoice->SetVolume(value);
}