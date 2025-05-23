#ifndef SYNTH_VOICECALLBACK_H
#define SYNTH_VOICECALLBACK_H
#include <xaudio2.h>
#include "Oscillator.h"
typedef void (Oscillator::*handlerType)(void *pBufferContext);

class VoiceCallback : public IXAudio2VoiceCallback {
public:
    VoiceCallback(handlerType handler, Oscillator* oscinstance);
    void OnBufferEnd(void *pBufferContext);

    handlerType methodHandler;
    Oscillator* osc;

    // Stub methods
    void OnStreamEnd() {}
    void OnVoiceProcessingPassEnd() {}
    void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
    void OnBufferStart(void *pBufferContext) {}
    void OnLoopEnd(void *pBufferContext) {}
    void OnVoiceError(void *pBufferContext, HRESULT Error) {}
};

#endif //SYNTH_VOICECALLBACK_H