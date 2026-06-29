#include "Crystal.h"

void Crystal::SetupInstrument() {
    mConfig.osc1Type = SynthUtils::WaveformType::Sine;
    mConfig.osc2Type = SynthUtils::WaveformType::Triangle;
    mConfig.oscMix   = 0.25f;
    mConfig.detune   = 0.012f;

    mConfig.attack  = 0.005f;
    mConfig.decay   = 1.8f;
    mConfig.sustain = 0.0f;
    mConfig.release = 2.5f;

    mConfig.filterCutoff    = 12000.0f;
    mConfig.filterResonance = 0.4f;
    mConfig.filterEnvAmount = 0.0f;

    mConfig.lfoRate   = 3.5f;
    mConfig.lfoAmount = 0.03f;

    mConfig.delayTime     = 0.35f;
    mConfig.delayFeedback = 0.6f;
    mConfig.delayMix      = 0.4f;

    mConfig.reverbSize = 0.95f;
    mConfig.reverbMix  = 0.6f;
    mConfig.volume = 1.0f;
}
