#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <iostream>

class SoundGenerator {
public:
    static bool GenerateAllAssets(const std::string& outputFolder = "assets/audio/");

    // Individual procedural sound synthesizer functions
    static std::vector<int16_t> GenerateMeow();
    static std::vector<int16_t> GenerateSqueak();
    static std::vector<int16_t> GenerateSlingshot();
    static std::vector<int16_t> GenerateLaser();
    static std::vector<int16_t> GenerateStun();
    static std::vector<int16_t> GenerateRoundClear();

    static bool SaveWAV(const std::string& filename, const std::vector<int16_t>& samples, int sampleRate = 44100);

private:
    static constexpr float PI = 3.14159265358979323846f;

    // Standard 8-bit retro waveform generators
    static float SquareWave(float phase) {
        return (std::fmod(phase, 2.0f * PI) < PI) ? 1.0f : -1.0f;
    }

    static float TriangleWave(float phase) {
        float p = std::fmod(phase, 2.0f * PI) / (2.0f * PI);
        return 2.0f * std::fabs(2.0f * (p - std::floor(p + 0.5f))) - 1.0f;
    }

    static float SineWave(float phase) {
        return std::sin(phase);
    }

    static float WhiteNoise() {
        return ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
    }
};
