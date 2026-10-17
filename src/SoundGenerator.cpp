#include "SoundGenerator.h"
#include <filesystem>

#pragma pack(push, 1)
struct WAVHeader {
    char     riff[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize = 0;
    char     wave[4] = {'W', 'A', 'V', 'E'};
    char     fmt[4]  = {'f', 'm', 't', ' '};
    uint32_t subchunk1Size = 16;
    uint16_t audioFormat = 1; // PCM Linear
    uint16_t numChannels = 1; // Mono
    uint32_t sampleRate = 44100;
    uint32_t byteRate = 44100 * 1 * 2;
    uint16_t blockAlign = 2;
    uint16_t bitsPerSample = 16;
    char     data[4] = {'d', 'a', 't', 'a'};
    uint32_t subchunk2Size = 0;
};
#pragma pack(pop)

bool SoundGenerator::SaveWAV(const std::string& filename, const std::vector<int16_t>& samples, int sampleRate) {
    WAVHeader header;
    header.sampleRate = sampleRate;
    header.numChannels = 1;
    header.bitsPerSample = 16;
    header.byteRate = sampleRate * header.numChannels * (header.bitsPerSample / 8);
    header.blockAlign = header.numChannels * (header.bitsPerSample / 8);
    header.subchunk2Size = static_cast<uint32_t>(samples.size() * sizeof(int16_t));
    header.chunkSize = 36 + header.subchunk2Size;

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "SoundGenerator: Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));
    file.write(reinterpret_cast<const char*>(samples.data()), header.subchunk2Size);
    file.close();

    std::cout << "Generated sound asset: " << filename << std::endl;
    return true;
}

// 1. Cat Meow: Frequency rises then falls smoothly
std::vector<int16_t> SoundGenerator::GenerateMeow() {
    int sampleRate = 44100;
    float duration = 0.35f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples;
        float freq = 400.0f + 350.0f * std::sin(t * PI);
        phase += (2.0f * PI * freq) / sampleRate;

        float env = std::sin(t * PI);
        float wave = 0.7f * SineWave(phase) + 0.3f * TriangleWave(phase);

        samples[i] = static_cast<int16_t>(wave * env * 24000.0f);
    }
    return samples;
}

// 2. Mouse Squeak: Short, high-pitched double chirp
std::vector<int16_t> SoundGenerator::GenerateSqueak() {
    int sampleRate = 44100;
    float duration = 0.18f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples; // Fixed static_cast typo

        float freq = 2000.0f + 600.0f * std::sin(t * PI * 4.0f);
        phase += (2.0f * PI * freq) / sampleRate;

        float env = (1.0f - t);
        float wave = SquareWave(phase);

        samples[i] = static_cast<int16_t>(wave * env * 12000.0f);
    }
    return samples;
}

// 3. Slingshot: Quick noise pop with pitch-drop snap
std::vector<int16_t> SoundGenerator::GenerateSlingshot() {
    int sampleRate = 44100;
    float duration = 0.12f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples;

        float freq = 600.0f * (1.0f - t) + 80.0f;
        phase += (2.0f * PI * freq) / sampleRate;

        float env = std::exp(-t * 12.0f);
        float wave = 0.5f * TriangleWave(phase) + 0.5f * WhiteNoise();

        samples[i] = static_cast<int16_t>(wave * env * 28000.0f);
    }
    return samples;
}

// 4. Laser: Downward retro square-wave sweep
std::vector<int16_t> SoundGenerator::GenerateLaser() {
    int sampleRate = 44100;
    float duration = 0.22f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples;

        float freq = 1400.0f * std::exp(-t * 6.0f) + 150.0f;
        phase += (2.0f * PI * freq) / sampleRate;

        float env = (1.0f - t);
        float wave = SquareWave(phase);

        samples[i] = static_cast<int16_t>(wave * env * 20000.0f);
    }
    return samples;
}

// 5. Stun: Low buzzing wobble
std::vector<int16_t> SoundGenerator::GenerateStun() {
    int sampleRate = 44100;
    float duration = 0.3f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples;

        float freq = 120.0f + 40.0f * SquareWave(t * PI * 30.0f);
        phase += (2.0f * PI * freq) / sampleRate;

        float env = (1.0f - t);
        float wave = SquareWave(phase); // Fixed call

        samples[i] = static_cast<int16_t>(wave * env * 22000.0f);
    }
    return samples;
}

// 6. Round Clear: Ascending 3-note arpeggio (C5 - E5 - G5)
std::vector<int16_t> SoundGenerator::GenerateRoundClear() {
    int sampleRate = 44100;
    float duration = 0.45f;
    int totalSamples = static_cast<int>(sampleRate * duration);
    std::vector<int16_t> samples(totalSamples);

    float phase = 0.0f;
    for (int i = 0; i < totalSamples; ++i) {
        float t = static_cast<float>(i) / totalSamples;

        float freq = 523.25f; // C5
        if (t > 0.33f && t <= 0.66f) freq = 659.25f; // E5
        else if (t > 0.66f)          freq = 783.99f; // G5

        phase += (2.0f * PI * freq) / sampleRate;

        float env = std::sin((fmod(t, 0.33f) / 0.33f) * PI);
        float wave = TriangleWave(phase);

        samples[i] = static_cast<int16_t>(wave * env * 24000.0f);
    }
    return samples;
}

bool SoundGenerator::GenerateAllAssets(const std::string& outputFolder) {
    // Ensure the output directory exists
    std::filesystem::create_directories(outputFolder);

    bool success = true;
    success &= SaveWAV(outputFolder + "meow.wav", GenerateMeow());
    success &= SaveWAV(outputFolder + "squeak.wav", GenerateSqueak());
    success &= SaveWAV(outputFolder + "slingshot.wav", GenerateSlingshot());
    success &= SaveWAV(outputFolder + "laser.wav", GenerateLaser());
    success &= SaveWAV(outputFolder + "stun.wav", GenerateStun());
    success &= SaveWAV(outputFolder + "round_clear.wav", GenerateRoundClear());
    return success;
}
