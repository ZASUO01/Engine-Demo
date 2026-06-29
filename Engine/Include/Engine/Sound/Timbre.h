#pragma once
#include <memory>
#include <vector>

#include "SynthUtils.h"
#include "Engine/Math/Math.h"

class Timbre {
public:
    struct Config {
        SynthUtils::WaveformType osc1Type = SynthUtils::WaveformType::Sine;
        SynthUtils::WaveformType osc2Type = SynthUtils::WaveformType::Sine;
        float oscMix   = 0.0f;
        float detune   = 0.0f;

        float lfoRate   = 0.0f;
        float lfoAmount = 0.0f;

        float filterCutoff      = 20000.0f;
        float filterResonance   = 0.0f;
        float filterEnvAmount   = 0.0f;

        float attack    = 0.0f;
        float decay     = 0.0f;
        float sustain   = 1.0f;
        float release   = 0.0f;

        float delayTime     = 0.0f;
        float delayFeedback = 0.0f;
        float delayMix      = 0.0f;

        float reverbSize    = 0.0f;
        float reverbMix     = 0.0f;
        float volume = 0.0f;
    };

    Timbre();
    virtual ~Timbre();

    virtual void SetupInstrument() = 0;

    template<typename T>
    static std::unique_ptr<T> CreateUnique() {
        static_assert(std::is_base_of_v<Timbre, T>, "T must derive from Timbre");
        auto instrument = std::make_unique<T>();
        instrument->SetupInstrument();

        return instrument;
    }

    // Sample processors
    void ApplyADSR(float& sampleL, float& sampleR, float sampleTimeStep, const float& timePlayed, float duration, bool isPlaying);
    void ApplyLFO(float& sampleL, float& sampleR, float sampleTimeStep);
    void ApplyLowPassFilter(float& sampleL, float& sampleR);
    void ApplyDelay(float& sampleL, float& sampleR);
    void ApplyReverb(float& sampleL, float& sampleR);

    [[nodiscard]] const Config& GetConfig() const { return mConfig; }
    void SetVolume(const float volume) { mConfig.volume = Math::Clamp(volume, 0.0f, 1.0f); }

protected:
    Config mConfig;

private:
    enum class EnvelopeState { Idle, Attack, Decay, Sustain, Release };

    void InitializeDelayBuffer();
    void InitializeReverbBuffer();

    EnvelopeState mEnvState;
    float mEnvVolume;
    float mEnvTime;
    float mLfoPhase;

    // Low pass filter
    float mPrevSampleL;
    float mPrevSampleR;

    // delay
    std::vector<float> mDelayBufferL;
    std::vector<float> mDelayBufferR;
    int mDelayWriteIndex = 0;

    // reverb
    std::vector<float> mReverbBufferL;
    std::vector<float> mReverbBufferR;
    int mReverbWriteIndex = 0;
};