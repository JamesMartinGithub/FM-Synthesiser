#include <functional>
#include "VoiceCallback.h"

VoiceCallback::VoiceCallback(handlerType handler, Synth::Oscillator* oscinstance) : methodHandler(handler), osc(oscinstance) {
}

void VoiceCallback::OnBufferEnd(void *pBufferContext) {
    std::invoke(methodHandler, osc, pBufferContext);
}