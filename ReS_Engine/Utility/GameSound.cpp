#include "GameSound.h"
#include <math.h>
#include "ReSUtility.h"
bool GameSound::Init()
{
    return true;
}

bool GameSound::Frame()
{
    FSystem->update();
    return true;
}

bool GameSound::Render()
{
    return true;
}

bool GameSound::Release()
{
    if (FSound) FSound->release();
    return true;
}

void GameSound::Pause()
{
    if (FChannel == nullptr) 
    {
        return;
    }
    bool pause;
    FChannel->getPaused(&pause);
    FChannel->setPaused(!pause);
}

void GameSound::VolumeAdj(bool adjust)
{
    if (adjust) 
    {
        Volume += 0.1f;
        Volume = std::min(1.0f, Volume);
        FChannel->setVolume(Volume);
    }
    else 
    {
        Volume -= 0.1f;
        Volume = std::max(0.0f, Volume);
        FChannel->setVolume(Volume);
    }
}

bool GameSound::Play(bool loop)
{
    if (IsPlay() == false) 
    {
        FMOD_RESULT hr = FSystem->playSound(FSound, nullptr, false, &FChannel);
        if (hr == FMOD_OK) 
        {
            Volume = 0.5f;
            FChannel->setVolume(Volume);
            SetLoop(loop);
        }
    }
    return true;
}

bool GameSound::PlayEffect(float volume, bool loop)
{
    FMOD_RESULT hr = FSystem->playSound(FSound, 0, false, &FChannel);
    if (hr == FMOD_OK) 
    {
        FChannel->setVolume(volume);
        SetLoop(loop);
    }
    return true;
}

void GameSound::Stop()
{
    FChannel->stop();
}

void GameSound::SetLoop(bool loop)
{
    if (loop) 
    {
        FSound->setMode(FMOD_LOOP_NORMAL);
    }
    else 
    {
        FSound->setMode(FMOD_LOOP_OFF);
    }
}

bool GameSound::IsPlay()
{
    bool playing = false;
    if (FChannel != nullptr) 
    {
        FChannel->isPlaying(&playing);
    }
    return playing;
}

void GameSound::SetVolume(float volume)
{
    FChannel->setVolume(Volume);
}

FMOD_RESULT GameSound::Load(FMOD::System* system, std::wstring filename)
{
    FSystem = system;
    FMOD_RESULT hr = FSystem->createSound(WstrToStr(filename).c_str(), FMOD_DEFAULT, nullptr, &FSound);
    return hr;
}

