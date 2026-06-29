#include "Engine/Sound/Synthesizer.h"
#include <SDL2/SDL_log.h>
#include "Engine/Math/Math.h"
#include "Engine/Sound/Timbre.h"

Synthesizer::Synthesizer(const int sampleRate)
:mSampleRate(sampleRate)
,mDeviceID(0)
,mTimbre(nullptr)
,mIsPlaying(false)
,mTimePlayed(0)
,mDuration(0)
,mFrequency(0)
,mPhaseL(0)
,mPhaseR(0)
{
    mSampleTimeStep = 1.0f / static_cast<float>(mSampleRate);
}

Synthesizer::~Synthesizer() {
    Shutdown();
}

bool Synthesizer::Init() {
    SDL_AudioSpec spec;
    SDL_zero(spec);

    spec.freq = mSampleRate;
    spec.format = AUDIO_F32;

    // Define stereo sound
    spec.channels = 2;

    // Two 512 size samples (L and R)
    spec.samples = CHUNK_SIZE;

    // using the SDL_AudioStream instead of old callbacks
    spec.callback = nullptr;
    spec.userdata = nullptr;

    mDeviceID = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);

    if (mDeviceID == 0) {
        SDL_Log("[SYNTHESIZER] Failed to open audio device");
        return false;
    }

    SDL_PauseAudioDevice(mDeviceID, 0);
    return true;
}

void Synthesizer::Update() {
    uint32_t queuedBytes = SDL_GetQueuedAudioSize(mDeviceID);
    while(queuedBytes + CHUNK_BYTE_SIZE <= SAFETY_QUEUE_BYTE_SIZE) {
        float buffer[CHUNK_SIZE];
        int bufferIndex = 0;

        for (int i = 0; i < CHUNK_UNIT_SIZE; ++i) {
            if (mIsPlaying) {
                mTimePlayed += mSampleTimeStep;
            }

            float sampleL = 0.0f;
            float sampleR = 0.0f;

            if (mTimbre) {
                GenerateRawSamples(sampleL, sampleR);
                mTimbre->ApplyLFO(sampleL, sampleR, mSampleTimeStep);
                mTimbre->ApplyADSR(sampleL, sampleR, mSampleTimeStep, mTimePlayed, mDuration, mIsPlaying);
                mTimbre->ApplyLowPassFilter(sampleL, sampleR);
                mTimbre->ApplyDelay(sampleL, sampleR);
                mTimbre->ApplyReverb(sampleL, sampleR);

                if (sampleL == 0.0f && sampleR == 0.0f && mTimePlayed >= mDuration) {
                    mIsPlaying = false;
                    mFrequency = 0.0f;
                    mPhaseL = 0.0f;
                    mPhaseR = 0.0f;
                }
            }

            buffer[bufferIndex++] = sampleL;
            buffer[bufferIndex++] = sampleR;
        }

        // Enqueue the chunk
        SDL_QueueAudio(mDeviceID, buffer, CHUNK_BYTE_SIZE);
        queuedBytes = SDL_GetQueuedAudioSize(mDeviceID);
    }
}

void Synthesizer::SetTimbre(std::unique_ptr<Timbre> timbre) {
    mTimbre = std::move(timbre);
}

void Synthesizer::PlayNote(const SynthUtils::Note note, const int octave, const float duration) {
    mFrequency = GetFrequencyFromNote(note, octave);

    if (mFrequency == 0.0f) {
        StopNote();
        return;
    }

    mDuration = duration;
    mTimePlayed = 0.0f;

    mPhaseL = 0.0f;
    mPhaseR = 0.0f;

    mIsPlaying = true;
}

void Synthesizer::StopNote() {
    mIsPlaying = false;

    mFrequency = 0.0f;
    mTimePlayed = 0.0f;
}

void Synthesizer::Shutdown() {
    if (mDeviceID != 0) {
        SDL_ClearQueuedAudio(mDeviceID);
        SDL_PauseAudioDevice(mDeviceID, 1);
        SDL_CloseAudioDevice(mDeviceID);
        mDeviceID = 0;
    }
}

void Synthesizer::GenerateRawSamples(float &sampleL, float &sampleR) {
    if (!mIsPlaying) {
        sampleL = 0.0f;
        sampleR = 0.0f;
        return;
    }

    const auto& cfg = mTimbre->GetConfig();

    const float leftFrequency = mFrequency - mFrequency * cfg.detune * 0.005f;
    const float rightFrequency = mFrequency + mFrequency * cfg.detune * 0.005f;

    mPhaseL += leftFrequency * mSampleTimeStep;
    if (mPhaseL >= 1.0f) {
        mPhaseL -= std::floor(mPhaseL);
    }

    mPhaseR += rightFrequency * mSampleTimeStep;
    if (mPhaseR >= 1.0f) {
        mPhaseR -= std::floor(mPhaseR);
    }

    float valueL = GenerateBasicWave(cfg.osc1Type, mPhaseL);
    float valueR = GenerateBasicWave(cfg.osc2Type, mPhaseR);

    valueL *= cfg.volume;
    valueR *= cfg.volume;

    sampleL = valueL;
    sampleR = valueR;
}

float Synthesizer::GetFrequencyFromNote(const SynthUtils::Note note, const int octave) {
    if (note == SynthUtils::Note::None || note == SynthUtils::Note::Count) {
        return 0.0f;
    }

    const int octaveClamp = Math::Clamp(octave, 2, 4);
    int exponent = octaveClamp - 2;

    static constexpr float secondOctaveFrequencies[] = {
        65.41f,
        69.30f,
        73.42f,
        77.78f,
        82.41f,
        87.31f,
        92.50f,
        98.00f,
        103.83f,
        110.00f,
        116.54f,
        123.47f
    };

    const int index = static_cast<int>(note);
    float freqBase = secondOctaveFrequencies[index];

    while (exponent--) {
        freqBase *= 2;
    }

    return freqBase;
}

float Synthesizer::GenerateBasicWave(const SynthUtils::WaveformType type, const float phase) {
    switch (type) {
        case SynthUtils::WaveformType::Sine:
            return Math::Sin(phase * Math::TwoPi);
        case SynthUtils::WaveformType::Square:
            return phase < 0.5f ? 1.0f : -1.0f;
        case SynthUtils::WaveformType::Sawtooth:
            return phase * 2.0f - 1.0f;
        case SynthUtils::WaveformType::Triangle:
            if (phase < 0.25f) return phase * 4.0f;
            if (phase < 0.75f) return 2.0f - phase * 4.0f;
            return phase * 4.0f - 4.0f;
    }
    return 0.0f;
}
