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
    pcmAudio1 = new BYTE[bufferSizeBytes + (bufferExtraSamples * 8)];
    pcmAudio2 = new BYTE[bufferSizeBytes + (bufferExtraSamples * 8)];
    GenerateAudio(pcmAudio1, &bufferSizeBytes);
    GenerateAudio(pcmAudio2, &bufferSizeBytes);
    buffer1.pAudioData = pcmAudio1;
    buffer2.pAudioData = pcmAudio2;

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

/// Populates an audio buffer with samples
void Oscillator::GenerateAudio(BYTE *pcmAudio, const DWORD *lengthInBytes) {
    pcm.bytes = pcmAudio;
    double thetaMod;
    float decValue;
    float decValueL;
    float decValueR;
    // Calculate buffer audio for both oscillators, then combine
    // Calculate oscillators in order such that modulatorOsc runs before carrierOsc
    for (int osc = (carrierOsc == 1) ? 0 : 1; osc < 2 && osc >= 0; osc+= (carrierOsc == 1) ? 1 : -1) {
        float a = 1.0f / (powf((float)voiceCount[osc], 0.5f));
        // Calculate buffer audio for individual oscillator
        for (int i = 0; i <= *lengthInBytes - (4 * channels); i += (4 * channels)) {
            float modulator = (osc == modulatorOsc) ? 0.0f :
                              std::lerp(0.0f, ((pcm.floats[i / 4] * 0.5f) + (pcm.floats[(i / 4) + 1] * 0.5f)), modulationAmount[osc]);
            decValueL = 0.0f;
            decValueR = 0.0f;
            // Calculate sample for each voice, then combine
            for (int v = 0; v < voiceCount[osc]; v++) {
                int phaseIndex = v + (osc * 8);
                float frequencyDiff = (voiceCount[osc] == 1) ? 0 : -detune[osc] + ((2 * detune[osc]) / ((float) voiceCount[osc] - 1)) * v;
                double voiceFrequency = oscFrequency[osc] + frequencyDiff;
                decValue = 0.0f;
                switch (waveform[osc]) {
                    case 0: // Sine
                        decValue += a * sin(phase[phaseIndex]);
                        break;
                    case 1: // Square
                        thetaMod = fmod(phase[phaseIndex], wavelength);
                        decValue += thetaMod > halfWavelength ? a * 0.6f : -a * 0.6f;
                        break;
                    case 2: // Triangle
                        thetaMod = fmod(phase[phaseIndex], wavelength);
                        decValue += a * (abs((thetaMod / halfWavelength) - 1) - 0.5f) * 2;
                        break;
                    case 3: // Sawtooth
                        thetaMod = fmod(phase[phaseIndex], wavelength);
                        decValue += a * ((thetaMod / wavelength) - 0.5f);
                        break;
                }
                // Amplitude Modulation
                if (osc == carrierOsc && modulationType[osc] == 2) {
                    decValue = decValue + abs(decValue * (modulator * 2.0f));
                }
                // Ring Modulation
                if (osc == carrierOsc && modulationType[osc] == 3) {
                    decValue = decValue + (decValue * (modulator * 2.0f));
                }
                // Increment phase
                if (modulationType[osc] != 1 && modulationType[osc] != 4) {
                    phase[phaseIndex] += (wavelength * voiceFrequency) / (double)sampleRate;
                } else {
                    // Frequency modulation
                    if (modulationType[osc] == 1) {
                        phase[phaseIndex] += ((wavelength * voiceFrequency) / (double)sampleRate) + modulator;
                    }
                    // Sync modulation
                    if (modulationType[osc] == 4) {
                        phase[phaseIndex] += (wavelength * (voiceFrequency * ((modulationAmount[osc] * 15) + 1))) / (double)sampleRate;
                    }
                }
                // Reset phase when wavelength passed
                if (modulationType[osc] != 4) {
                    if (phase[phaseIndex] > wavelength) {
                        phase[phaseIndex] -= wavelength;
                    }
                } else {
                    // Sync modulation increases wavelengths per reset
                    if (phase[phaseIndex] > wavelength * ((modulationAmount[osc] * 15) + 1)) {
                        phase[phaseIndex] -= wavelength * ((modulationAmount[osc] * 15) + 1);
                    }
                }
                // Add sample value to LR channels
                decValueL += decValue * std::lerp(1, std::lerp(1, 0, ((frequencyDiff / maxDetune) + 1) / 2.0f), unisonWidth[osc]);
                decValueR += decValue * std::lerp(1, std::lerp(0, 1, ((frequencyDiff / maxDetune) + 1) / 2.0f), unisonWidth[osc]);
            }
            /*
            // Slew limiter to prevent clicks
            if (abs(lastSampleL[osc] - decValueL) > 0.15f) {
                decValueL = std::lerp(lastSampleL[osc], decValueL, 0.4f);
            }
            lastSampleL[osc] = decValueL;
            if (abs(lastSampleR[osc] - decValueR) > 0.15f) {
                decValueR = std::lerp(lastSampleR[osc], decValueR, 0.4f);
            }
            lastSampleR[osc] = decValueR;
             */
            // Copy sample to buffer array
            if (channels == 1) {
                decValue = (decValueL * 0.5f) + (decValueR * 0.5f);
                // Combine oscillators A and B
                if (osc == carrierOsc) {
                    decValue += pcm.floats[i / 8];
                }
                // Copy to pcm buffer
                memcpy(&pcmAudio[i], &decValue, 4);
            } else {
                // Combine oscillators A and B and apply volumes
                if (osc == carrierOsc) {
                    decValueL = (decValueL * amplitude[carrierOsc]) + (pcm.floats[i / 4] * amplitude[modulatorOsc]);
                    decValueR = (decValueR * amplitude[carrierOsc]) + (pcm.floats[(i / 4) + 1] * amplitude[modulatorOsc]);
                }
                // Copy to pcm buffer
                memcpy(&pcmAudio[i], &decValueL, 4);
                memcpy(&pcmAudio[i + 4], &decValueR, 4);
            }
        }
    }
}

/// Re-populates and adds the buffer that just ended
void Oscillator::AddBuffer(void *pBufferContext) {
    Context context;
    context.ptr = pBufferContext;
    if (context.id == 1) {
        GenerateAudio(pcmAudio1, &bufferSizeBytes);
        sourceVoice->SubmitSourceBuffer(&buffer1);
        controller->RenderWaveform(pcmAudio1);
    } else {
        GenerateAudio(pcmAudio2, &bufferSizeBytes);
        sourceVoice->SubmitSourceBuffer(&buffer2);
        controller->RenderWaveform(pcmAudio2);
    }
}

void Oscillator::SetWaveform(int pId, bool isA) {
    waveform[isA ? 1 : 0] = pId;
}

void Oscillator::SetVolume(float pVolume, bool isA) {
    amplitude[isA ? 1 : 0] = pVolume;
}

void Oscillator::SetUnison(int pVoiceCount, bool isA) {
    voiceCount[isA ? 1 : 0] = pVoiceCount;
}

void Oscillator::SetDetune(int pDetunePercent, bool isA) {
    detune[isA ? 1 : 0] = ((float)pDetunePercent * maxDetune) / 100;
}

void Oscillator::SetUnisonWidth(int pWidthPercent, bool isA) {
    unisonWidth[isA ? 1 : 0] = (float)pWidthPercent / 100.0f;
}

void Oscillator::SetModulationType(int pModulationType, bool isA) {
    modulationType[isA ? 1 : 0] = pModulationType;
    if (pModulationType == 1 || pModulationType == 2 || pModulationType == 3) {
        modulatorOsc = isA ? 0 : 1;
        carrierOsc = isA ? 1 : 0;
    }
}

void Oscillator::SetModulationValue(float pModulationValue, bool isA) {
    modulationAmount[isA ? 1 : 0] = pModulationValue;
}

void Oscillator::SetOctave(int pOctave, bool isA) {
    int i = isA ? 1 : 0;
    octave[i] = pOctave;
    oscFrequency[i] = baseFrequency * powf(2, ((octave[i] * 12 + semitone[i]) / 12.0f));
}

void Oscillator::SetSemitone(int pSemitone, bool isA) {
    int i = isA ? 1 : 0;
    semitone[i] = pSemitone;
    oscFrequency[i] = baseFrequency * powf(2, ((octave[i] * 12 + semitone[i]) / 12.0f));
}