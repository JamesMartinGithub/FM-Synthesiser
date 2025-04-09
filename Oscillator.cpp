#include "Oscillator.h"
#include "VoiceCallback.h"
#include "Controller.h"

Oscillator::Oscillator(int pId, Controller* pController, AudioSettings pSettings, IXAudio2* pXaudio2, float pFrequency)
    : id(pId)
    , controller(pController)
    , channels(pSettings.channels)
    , sampleRate(pSettings.sampleRate)
    , resolution(pSettings.resolution)
    , bufferSize(pSettings.bufferSize)
    , bytesPerSec((sampleRate * channels * resolution) / 8)
    , blockAlign((channels * resolution) / 8)
    , bufferSizeBytes(bufferSize * 8)
    , xAudio2(pXaudio2)
    , baseFrequency(pFrequency) {
    // Create format object
    const WAVEFORMATEX format{WAVE_FORMAT_IEEE_FLOAT, channels, sampleRate, bytesPerSec, blockAlign, resolution, 0};

    // Create and assign buffer arrays
    nextAdsrPhase = floor(sampleRate * adsr.attack);
    pcmAudio1 = new BYTE[bufferSizeBytes];
    pcmAudio2 = new BYTE[bufferSizeBytes];
    buffer1.pAudioData = pcmAudio1;
    buffer2.pAudioData = pcmAudio2;

    // Create submix voice
    xAudio2->CreateSubmixVoice(&submixVoice, channels, sampleRate, 0, 0);
    submixVoice->SetVolume(0.5f);
    XAUDIO2_SEND_DESCRIPTOR sendDescriptor = {0, submixVoice};
    XAUDIO2_VOICE_SENDS sendList = {1, &sendDescriptor};

    // Create source voice
    voiceCallback = new VoiceCallback(&Oscillator::AddBuffer, this);
    xAudio2->CreateSourceVoice(&sourceVoice, &format, 0, 2.0f, voiceCallback, &sendList);

    // Enable buffer playback
    sourceVoice->Start(0);
}

Oscillator::~Oscillator() {
    std::cout << "Destructor called" << "\n";
    sourceVoice->Stop();
    voiceCallback->osc = nullptr;
    delete voiceCallback;
    sourceVoice->DestroyVoice();
    submixVoice->DestroyVoice();
    sourceVoice = nullptr;
    delete[] pcmAudio1;
    delete[] pcmAudio2;
}

/// Populates an audio buffer with samples
void Oscillator::GenerateAudio(BYTE *pcmAudio, const DWORD *lengthInBytes) {
    pcm.bytes = pcmAudio;
    double phaseMod;
    float adsrAmplitude;
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
                        phaseMod = fmod(phase[phaseIndex], wavelength);
                        decValue += phaseMod > halfWavelength ? a * 0.6f : -a * 0.6f;
                        break;
                    case 2: // Triangle
                        phaseMod = fmod(phase[phaseIndex], wavelength);
                        decValue += a * (abs((phaseMod / halfWavelength) - 1) - 0.5f) * 2;
                        break;
                    case 3: // Sawtooth
                        phaseMod = fmod(phase[phaseIndex], wavelength);
                        decValue += a * ((phaseMod / wavelength) - 0.5f);
                        break;
                }
                // Amplitude modulation
                if (osc == carrierOsc && modulationType[osc] == 2) {
                    decValue = decValue + abs(decValue * (modulator * 2.0f));
                }
                // Ring modulation
                if (osc == carrierOsc && modulationType[osc] == 3) {
                    decValue = decValue + (decValue * (modulator * 2.0f));
                }
                // Increment phase
                if (modulationType[osc] != 1 && modulationType[osc] != 4 && modulationType[osc] != 5) {
                    phase[phaseIndex] += (wavelength * voiceFrequency) / (double)sampleRate;
                } else {
                    // Frequency modulation
                    if (modulationType[osc] == 1) {
                        phase[phaseIndex] += ((wavelength * voiceFrequency) / (double)sampleRate) + modulator;
                    }
                    // Sync modulation
                    if (modulationType[osc] == 4 || modulationType[osc] == 5) {
                        // Sync modulation with softening
                        if (modulationType[osc] == 5) {
                            decValue *= sin((phase[phaseIndex] / (wavelength * ((modulationAmount[osc] * 15) + 1))) * halfWavelength);
                        }
                        phase[phaseIndex] += (wavelength * (voiceFrequency * ((modulationAmount[osc] * 15) + 1))) / (double)sampleRate;
                    }
                }
                // Reset phase when wavelength passed
                if (modulationType[osc] != 4 && modulationType[osc] != 5) {
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
            // Quantize modulation
            if (modulationType[osc] == 6) {
                // Reduce samplerate to specified division
                int quantizeStep = floor(phase[osc * 8] / ((wavelength / std::lerp((float)wavelength * 32.0f, 2.0f, modulationAmount[osc])) + modulationAmount[osc]));
                if (lastQuantizeStep[osc] != quantizeStep) {
                    lastQuantizeStep[osc] = quantizeStep;
                    lastQuantizeValueL[osc] = decValueL;
                    lastQuantizeValueR[osc] = decValueR;
                }
                // Replace sample with quantized version
                decValueL = lastQuantizeValueL[osc];
                decValueR = lastQuantizeValueR[osc];
            }
            // Apply panning
            decValueL = std::lerp(decValueL, 0.0f, pan[osc]);
            decValueR = std::lerp(0.0f, decValueR, pan[osc]);
            if (osc == carrierOsc) {
                // Combine oscillators A and B and apply volumes
                if (channels == 1) {
                    decValueL = (decValueL * amplitude[carrierOsc]) + (pcm.floats[i / 4] * amplitude[modulatorOsc]);
                    decValueR = (decValueR * amplitude[carrierOsc]) + (pcm.floats[i / 4] * amplitude[modulatorOsc]);
                } else {
                    decValueL = (decValueL * amplitude[carrierOsc]) + (pcm.floats[i / 4] * amplitude[modulatorOsc]);
                    decValueR = (decValueR * amplitude[carrierOsc]) + (pcm.floats[(i / 4) + 1] * amplitude[modulatorOsc]);
                }
                // Determine adsr phase
                if (previousSamples >= nextAdsrPhase && adsrPhase % 2 == 0) {
                    adsrPhase++;
                    lastAdsrPhase = nextAdsrPhase;
                    switch (adsrPhase) {
                        case 1:
                            nextAdsrPhase = floor(sampleRate * (adsr.attack + adsr.decay));
                            break;
                        case 2:
                            //nextAdsrPhase = floor(sampleRate * (adsr.attack + adsr.decay + adsr.release));
                            //releaseFromAmplitude = adsr.sustain;
                            break;
                        case 3:
                            sourceVoice->Discontinuity();
                            enabled = false;
                            controller->CleanupOscillator(id);
                            break;
                    }
                }
                // Apply adsr amplitude:
                switch (adsrPhase) {
                    case 0:
                        adsrAmplitude = std::lerp(0.0f, 1.0f, (float)previousSamples / (float)nextAdsrPhase);
                        decValueL *= adsrAmplitude;
                        decValueR *= adsrAmplitude;
                        break;
                    case 1:
                        adsrAmplitude = std::clamp(std::lerp(1.0f, adsr.sustain,(float)(previousSamples - lastAdsrPhase) / (float)(nextAdsrPhase - lastAdsrPhase)), adsr.sustain, 1.0f);
                        decValueL *= adsrAmplitude;
                        decValueR *= adsrAmplitude;
                        break;
                    case 2:
                        adsrAmplitude = std::clamp(std::lerp(releaseFromAmplitude, 0.0f, (float)(previousSamples - lastAdsrPhase) / (float)(nextAdsrPhase - lastAdsrPhase)), 0.0f, releaseFromAmplitude);
                        decValueL *= adsrAmplitude;
                        decValueR *= adsrAmplitude;
                        break;
                    case 3:
                        decValueL = 0.0f;
                        decValueR = 0.0f;
                }
                previousSamples++;
            }
            // Copy sample to buffer array
            if (channels == 1) {
                // Combine L/R channels
                decValue = (decValueL * 0.5f) + (decValueR * 0.5f);
                // Copy to pcm buffer
                memcpy(&pcmAudio[i], &decValue, 4);
            } else {
                // Copy to pcm buffer
                memcpy(&pcmAudio[i], &decValueL, 4);
                memcpy(&pcmAudio[i + 4], &decValueR, 4);
            }
        }
    }
    if (adsrPhase != 2) {
        releaseFromAmplitude = adsrAmplitude;
    }
}

/// Re-populates and adds the buffer that just ended
void Oscillator::AddBuffer(void *pBufferContext) {
    if (enabled) {
        Context context;
        context.ptr = pBufferContext;
        if (context.id == 1) {
            GenerateAudio(pcmAudio1, &bufferSizeBytes);
            sourceVoice->SubmitSourceBuffer(&buffer1);
            if (id == 0) {
                controller->RenderWaveform(pcmAudio1);
            }
        } else {
            GenerateAudio(pcmAudio2, &bufferSizeBytes);
            sourceVoice->SubmitSourceBuffer(&buffer2);
            if (id == 0) {
                controller->RenderWaveform(pcmAudio2);
            }
        }
    }
}

/// Starts audio generation and output
void Oscillator::Start(float pFrequency) {
    SetFrequency(pFrequency);
    // Reset oscillator state
    for (int i = 0; i < 16; i++) {
        phase[i] = 0.0;
    }
    lastQuantizeValueL[0] = 0.0f;
    lastQuantizeValueL[1] = 0.0f;
    lastQuantizeValueR[0] = 0.0f;
    lastQuantizeValueR[1] = 0.0f;
    previousSamples = 0;
    adsrPhase = 0;
    nextAdsrPhase = floor(sampleRate * adsr.attack);
    // Start audio output
    GenerateAudio(pcmAudio1, &bufferSizeBytes);
    sourceVoice->SubmitSourceBuffer(&buffer1);
    AddBuffer((void*)2);
    enabled = true;
}

/// Sets adsr phase to release, allowing note to end
void Oscillator::Release() {
    if (adsrPhase != 2) {
        adsrPhase = 2;
        lastAdsrPhase = nextAdsrPhase;
        nextAdsrPhase = lastAdsrPhase + floor(sampleRate * adsr.release);
        previousSamples = lastAdsrPhase;
    }
}

void Oscillator::SetFrequency(float pFrequency) {
    baseFrequency = pFrequency;
    for (int i = 0; i < 2; i++) {
        oscFrequency[i] = baseFrequency * powf(2, ((float)(octave[i] * 12 + semitone[i]) / 12.0f));
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
    oscFrequency[i] = baseFrequency * powf(2, ((float)(octave[i] * 12 + semitone[i]) / 12.0f));
}

void Oscillator::SetSemitone(int pSemitone, bool isA) {
    int i = isA ? 1 : 0;
    semitone[i] = pSemitone;
    oscFrequency[i] = baseFrequency * powf(2, ((float)(octave[i] * 12 + semitone[i]) / 12.0f));
}

void Oscillator::SetPan(float pPan, bool isA) {
    pan[isA ? 1 : 0] = pPan;
}

void Oscillator::SetADSR(float pValue, char pFieldId) {
    switch (pFieldId) {
        case 'A':
            adsr.attack = pValue; break;
        case 'D':
            adsr.decay = pValue; break;
        case 'S':
            adsr.sustain = pValue; break;
        case 'R':
            adsr.release = pValue; break;
    }
}