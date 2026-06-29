#pragma once
#include <SDL2/SDL_audio.h>
#include "Engine/Sound/SynthUtils.h"
#include <memory>

class Timbre;

class Synthesizer {
public:
    explicit Synthesizer(int sampleRate = 44100);
    ~Synthesizer();

    [[nodiscard]] bool Init();
    void Update();

    void SetTimbre(std::unique_ptr<Timbre> timbre);
    void PlayNote(SynthUtils::Note note, int octave, float duration);
    void StopNote();

private:
    void Shutdown();
    void GenerateRawSamples(float& sampleL, float& sampleR);
    static float GetFrequencyFromNote(SynthUtils::Note note, int octave);
    static float GenerateBasicWave(SynthUtils::WaveformType type, float phase);

    int mSampleRate;
    float mSampleTimeStep;
    SDL_AudioDeviceID mDeviceID;

    std::unique_ptr<Timbre> mTimbre;
    bool mIsPlaying;
    float mTimePlayed;
    float mDuration;
    float mFrequency;
    float mPhaseL;
    float mPhaseR;

    // Constants
    static constexpr int SAFETY_QUEUE_BYTE_SIZE =  4096 * 2 * sizeof(float);// 4096 bytes * 2 channels * float bytesize
    static constexpr int CHUNK_UNIT_SIZE = 512; // 512 bytes
    static constexpr int CHUNK_SIZE = CHUNK_UNIT_SIZE * 2; // 512 bytes * 2 channels
    static  constexpr int CHUNK_BYTE_SIZE = CHUNK_SIZE * sizeof(float); // 512 bytes * 2 channels * float bytesize
};



