#include "Engine/Sound/Timbre.h"

Timbre::Timbre()
:mEnvState(EnvelopeState::Idle)
,mEnvVolume(0.0f)
,mEnvTime(0.0f)
,mLfoPhase(0.0f)
,mPrevSampleL(0.0f)
,mPrevSampleR(0.0f)
{}

Timbre::~Timbre() = default;

void Timbre::ApplyADSR(float &sampleL, float &sampleR, const float sampleTimeStep, const float &timePlayed, const float duration, const bool isPlaying) {
    if (isPlaying && mEnvState == EnvelopeState::Idle) {
        mEnvState = EnvelopeState::Attack;
        mEnvTime = 0.0f;
    }

    if ((timePlayed >= duration || !isPlaying) && mEnvState != EnvelopeState::Release && mEnvState != EnvelopeState::Idle) {
        mEnvState = EnvelopeState::Release;
        mEnvTime = 0.0f;
    }

    if (mEnvState != EnvelopeState::Idle) {
        mEnvTime += sampleTimeStep;
    }

    switch (mEnvState) {
        case EnvelopeState::Attack:
            if (mConfig.attack > 0.0f) {

                mEnvVolume = mEnvTime / mConfig.attack;

                if (mEnvTime >= mConfig.attack) {
                    mEnvState = EnvelopeState::Decay;
                    mEnvTime = 0.0f;
                    mEnvVolume = 1.0f;
                }
            } else {
                mEnvVolume = 1.0f;
                mEnvState = EnvelopeState::Decay;
                mEnvTime = 0.0f;
            }
            break;

        case EnvelopeState::Decay:
            if (mConfig.decay > 0.0f) {

                const float progresso = mEnvTime / mConfig.decay;
                mEnvVolume = 1.0f - progresso * (1.0f - mConfig.sustain);


                if (mEnvTime >= mConfig.decay) {
                    mEnvState = EnvelopeState::Sustain;
                    mEnvVolume = mConfig.sustain;
                }
            } else {
                mEnvVolume = mConfig.sustain;
                mEnvState = EnvelopeState::Sustain;
            }
            break;

        case EnvelopeState::Sustain:

            mEnvVolume = mConfig.sustain;
            break;

        case EnvelopeState::Release:
            if (mConfig.release > 0.0f) {

                const float progress = mEnvTime / mConfig.release;
                mEnvVolume = mConfig.sustain * (1.0f - progress);


                if (mEnvTime >= mConfig.release || mEnvVolume <= 0.0f) {
                    mEnvVolume = 0.0f;
                    mEnvState = EnvelopeState::Idle;
                }
            } else {
                mEnvVolume = 0.0f;
                mEnvState = EnvelopeState::Idle;
            }
            break;

        case EnvelopeState::Idle:
            mEnvVolume = 0.0f;
            break;
    }


    mEnvVolume = Math::Clamp(mEnvVolume, 0.0f, 1.0f);


    sampleL *= mEnvVolume;
    sampleR *= mEnvVolume;
}

void Timbre::ApplyLFO(float &sampleL, float &sampleR, const float sampleTimeStep) {
    if (mConfig.lfoAmount <= 0.0f || mConfig.lfoRate <= 0.0f) {
        return;
    }

    mLfoPhase += mConfig.lfoRate * sampleTimeStep;
    if (mLfoPhase >= 1.0f) {
        mLfoPhase -= std::floor(mLfoPhase);
    }

    const float lfoWave = Math::Sin(mLfoPhase * Math::TwoPi);
    const float volumeMultiplicator = 1.0f - Math::Abs(lfoWave) * mConfig.lfoAmount * 0.5f;

    sampleL *= volumeMultiplicator;
    sampleR *= volumeMultiplicator;
}

void Timbre::ApplyLowPassFilter(float &sampleL, float &sampleR) {
    if (mConfig.filterCutoff >= 20000.0f) {
        return;
    }

    const float cutoffClamped = Math::Clamp(mConfig.filterCutoff, 20.0f, 20000.0f);
    const float omega = Math::TwoPi * cutoffClamped / 44100.0f;
    const float alpha = omega / (omega + 1.0f);

    sampleL = mPrevSampleL + alpha * (sampleL - mPrevSampleL);
    sampleR = mPrevSampleR + alpha * (sampleR - mPrevSampleR);

    mPrevSampleL = sampleL;
    mPrevSampleR = sampleR;
}

void Timbre::ApplyDelay(float &sampleL, float &sampleR) {
    InitializeDelayBuffer();

    if (mConfig.delayMix <= 0.0f || mConfig.delayTime <= 0.0f) {
        return;
    }
    const size_t bufferSize = mDelayBufferL.size();
    int delaySamples = static_cast<int>(mConfig.delayTime * 44100.0f);

    if (delaySamples >= static_cast<int>(bufferSize)) {
        delaySamples = static_cast<int>(bufferSize) - 1;
    }

    int readIndex = mDelayWriteIndex - delaySamples;
    if (readIndex < 0) {
        readIndex += static_cast<int>(bufferSize);
    }

    const float ecoL = mDelayBufferL[readIndex];
    const float ecoR = mDelayBufferR[readIndex];

    mDelayBufferL[mDelayWriteIndex] = sampleL + ecoL * mConfig.delayFeedback;
    mDelayBufferR[mDelayWriteIndex] = sampleR + ecoR * mConfig.delayFeedback;

    mDelayWriteIndex++;
    if (mDelayWriteIndex >= static_cast<int>(bufferSize)) {
        mDelayWriteIndex = 0;
    }

    sampleL = sampleL * (1.0f - mConfig.delayMix) + ecoL * mConfig.delayMix;
    sampleR = sampleR * (1.0f - mConfig.delayMix) + ecoR * mConfig.delayMix;
}

void Timbre::ApplyReverb(float& sampleL, float& sampleR) {
    InitializeReverbBuffer();

    if (mConfig.reverbMix <= 0.0f || mConfig.reverbSize <= 0.0f) {
        return;
    }

    const size_t bufferSize = mReverbBufferL.size();
    const float g = Math::Clamp(mConfig.reverbSize, 0.0f, 0.85f);

    const float delayedL = mReverbBufferL[mReverbWriteIndex];
    const float delayedR = mReverbBufferR[mReverbWriteIndex];

    const float reverbOutL = -g * sampleL + delayedL;
    const float reverbOutR = -g * sampleR + delayedR;

    mReverbBufferL[mReverbWriteIndex] = sampleL + g * delayedL;
    mReverbBufferR[mReverbWriteIndex] = sampleR + g * delayedR;

    mReverbWriteIndex++;
    if (mReverbWriteIndex >= static_cast<int>(bufferSize)) {
        mReverbWriteIndex = 0;
    }

    sampleL = sampleL * (1.0f - mConfig.reverbMix) + reverbOutL * mConfig.reverbMix;
    sampleR = sampleR * (1.0f - mConfig.reverbMix) + reverbOutR * mConfig.reverbMix;
}

void Timbre::InitializeDelayBuffer() {
    if (!mDelayBufferL.empty()) return;

    constexpr size_t bufferSize = 44100 * 2;
    mDelayBufferL.resize(bufferSize, 0.0f);
    mDelayBufferR.resize(bufferSize, 0.0f);
    mDelayWriteIndex = 0;
}

void Timbre::InitializeReverbBuffer() {
    if (!mReverbBufferL.empty()) return;

    constexpr size_t bufferSize = 1500;
    mReverbBufferL.resize(bufferSize, 0.0f);
    mReverbBufferR.resize(bufferSize, 0.0f);
    mReverbWriteIndex = 0;
}

