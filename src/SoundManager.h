#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

enum class SoundEffect {
    Meow,
    Squeak,
    Slingshot,
    Laser,
    Stun,
    RoundClear
};

class SoundManager {
public:
    static SoundManager& Instance();
    void StopAll();

    bool Initialize();
    void CleanUp();

    void LoadWAV(SoundEffect effect, const std::string& filePath);
    void Play(SoundEffect effect);

private:
    SoundManager() = default;
    ~SoundManager() = default;

    SDL_AudioStream* audioStream = nullptr;

    struct SoundBuffer {
        Uint8* buffer = nullptr;
        Uint32 length = 0;
    };

    std::unordered_map<SoundEffect, SoundBuffer> soundLibrary;
    bool isAudioReady = false;
};
