#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::Instance() {
    static SoundManager instance;
    return instance;
}

bool SoundManager::Initialize() {
    SDL_AudioSpec audioSpec;
    SDL_zero(audioSpec);
    audioSpec.freq = 44100;
    audioSpec.format = SDL_AUDIO_S16LE;
    audioSpec.channels = 1;

    // Open an audio stream directly connected to default playback device
    audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, nullptr, nullptr);
    if (!audioStream) {
        std::cerr << "SDL_OpenAudioDeviceStream Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_ResumeAudioStreamDevice(audioStream);
    isAudioReady = true;
    std::cout << "SDL3 Audio Subsystem Initialized Successfully!" << std::endl;
    return true;
}

void SoundManager::StopAll() {
    if (audioStream) {
        SDL_ClearAudioStream(audioStream);
    }
}

void SoundManager::LoadWAV(SoundEffect effect, const std::string& filePath) {
    if (!isAudioReady) return;

    SDL_AudioSpec wavSpec;
    Uint8* wavBuffer = nullptr;
    Uint32 wavLength = 0;

    if (!SDL_LoadWAV(filePath.c_str(), &wavSpec, &wavBuffer, &wavLength)) {
        std::cerr << "Failed to load WAV file: " << filePath << " | SDL Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (soundLibrary.find(effect) != soundLibrary.end()) {
        SDL_free(soundLibrary[effect].buffer);
    }

    soundLibrary[effect] = { wavBuffer, wavLength };
    std::cout << "Loaded Sound Effect: " << filePath << std::endl;
}

void SoundManager::Play(SoundEffect effect) {
    if (!isAudioReady || !audioStream) return;

    auto it = soundLibrary.find(effect);
    if (it != soundLibrary.end() && it->second.buffer != nullptr) {
        // Clear old sound queue so previous sounds stop cleanly
        SDL_ClearAudioStream(audioStream);

        SDL_PutAudioStreamData(audioStream, it->second.buffer, it->second.length);
        SDL_ResumeAudioStreamDevice(audioStream);
    }
}

void SoundManager::CleanUp() {
    for (auto& pair : soundLibrary) {
        if (pair.second.buffer) {
            SDL_free(pair.second.buffer);
        }
    }
    soundLibrary.clear();

    if (audioStream) {
        SDL_DestroyAudioStream(audioStream);
        audioStream = nullptr;
    }
    isAudioReady = false;
}
