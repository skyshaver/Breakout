#include "AudioEngine.h"

Implementation::Implementation()
{
    studioSystem = nullptr;
    AudioEngine::errorCheck(FMOD::Studio::System::create(&studioSystem));
    AudioEngine::errorCheck(studioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    system = nullptr;
    AudioEngine::errorCheck(studioSystem->getCoreSystem(&system));
}

Implementation::~Implementation()
{
    AudioEngine::errorCheck(studioSystem->unloadAll());
    AudioEngine::errorCheck(studioSystem->release());
}

void Implementation::update() {
    std::vector<ChannelMap::iterator> stoppedChannels;
    for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it)
    {
        bool isPlaying = false;
        it->second->isPlaying(&isPlaying);
        if (!isPlaying)
        {
            stoppedChannels.push_back(it);
        }
    }
    for (auto& it : stoppedChannels)
    {
        channels.erase(it);
    }
    AudioEngine::errorCheck(studioSystem->update());
}

std::unique_ptr<Implementation> implementation;

void AudioEngine::init()
{
    implementation = std::make_unique<Implementation>();
}

void AudioEngine::update()
{
    implementation->update();
}

int AudioEngine::errorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    // cout << "FMOD all good" << endl;
    return 0;
}

void AudioEngine::loadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
{
    if (implementation->banks.find(bankName) != implementation->banks.end())
    {
        return;
    }
    FMOD::Studio::Bank* bank;
    AudioEngine::errorCheck(implementation->studioSystem->loadBankFile(bankName.c_str(), flags, &bank));
    if (bank)
    {
        implementation->banks[bankName] = bank;
    }
}

void AudioEngine::loadEvent(const std::string& eventName)
{
    if (implementation->events.find(eventName) != implementation->events.end())
    {
        return;
    }

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    AudioEngine::errorCheck(implementation->studioSystem->getEvent(eventName.c_str(), &eventDescription));
    if (eventDescription)
    {
        FMOD::Studio::EventInstance* eventInstance = nullptr;
        AudioEngine::errorCheck(eventDescription->createInstance(&eventInstance));
        if (eventInstance)
        {
            implementation->events[eventName] = eventInstance;
        }
    }
}

void AudioEngine::loadSound(const std::string& soundPath, FMOD_MODE mode)
{
    
    if (implementation->sounds.find(soundPath) != implementation->sounds.end())
    {
        return;
    }

    /*FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3D ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;*/

    FMOD::Sound* sound = nullptr;
    AudioEngine::errorCheck(implementation->system->createSound(soundPath.c_str(), mode, nullptr, &sound));
    if (sound)
    {
        implementation->sounds[soundPath] = sound;
    }

}

void AudioEngine::loadSound(const std::string& soundPath, const std::string& soundName, FMOD_MODE mode)
{
    if (implementation->sounds.find(soundName) != implementation->sounds.end())
    {
       
        return;
    }
    /*std::cout << "oops" << '\n';
    FMOD_MODE mode = FMOD_DEFAULT;
    mode |= is3D ? FMOD_3D : FMOD_2D;
    mode |= isLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    mode |= isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;*/

    FMOD::Sound* sound = nullptr;
    AudioEngine::errorCheck(implementation->system->createSound(soundPath.c_str(), mode, nullptr, &sound));
    if (sound)
    {
        implementation->sounds[soundName] = sound;
    }
}

void AudioEngine::unLoadSound(const std::string& strSoundName)
{
    auto it = implementation->sounds.find(strSoundName);
    if (it == implementation->sounds.end())
    {
        return;
    }

    AudioEngine::errorCheck(it->second->release());
    implementation->sounds.erase(it);
}

int AudioEngine::playSound(const std::string& soundName, float volumedB, const glm::vec3 pos)
{
    int channelId = implementation->nextChannelId++;
    auto it = implementation->sounds.find(soundName);
    if (it == implementation->sounds.end())
    {
        loadSound(soundName);
        it = implementation->sounds.find(soundName);
        if (it == implementation->sounds.end())
        {
            return channelId;
        }
    }
    
    FMOD::Channel* channel = nullptr;
    AudioEngine::errorCheck(implementation->system->playSound(it->second, nullptr, true, &channel));

    if (channel)
    {
        FMOD_MODE currMode;
        it->second->getMode(&currMode);
        if (currMode & FMOD_3D)
        {
            FMOD_VECTOR position = vectorToFmod(pos);
            AudioEngine::errorCheck(channel->set3DAttributes(&position, nullptr));
        }
        AudioEngine::errorCheck(channel->setVolume(dBToVolume(volumedB)));
        AudioEngine::errorCheck(channel->setPaused(false));
        implementation->channels[channelId] = channel;
    }

    // debugging
    /*static int soundsPlayed = 0;
    soundsPlayed++;
    if (soundsPlayed > 5)
    {
        for (const auto& [key, value] : implementation->channels)
        {
            std::cout << "key: " << key << " value: " << value << '\n';
        }
    }*/

    return channelId;
}

void AudioEngine::playEvent(const std::string& eventName)
{
    auto it = implementation->events.find(eventName);
    if (it == implementation->events.end())
    {
        loadEvent(eventName);
        it = implementation->events.find(eventName);
        if (it == implementation->events.end())
        {
            return;
        }
    }
    it->second->start();
}

void AudioEngine::stopEvent(const std::string& eventName, bool isImmediate)
{
    auto it = implementation->events.find(eventName);
    if (it == implementation->events.end())
    {
        return;
    }

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = isImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    AudioEngine::errorCheck(it->second->stop(eMode));
}

// ParameterInstance is deprecated, trying to return the event parameter with an out value  using FMOD::Studio::EventInstance::getParameterByName
void AudioEngine::geteventParameter(const std::string& eventName, const std::string& eventParameter, float* parameter)
{
    auto it = implementation->events.find(eventName);
    if (it == implementation->events.end())
    {
        return;
    }

    //FMOD::Studio::ParameterInstance* parameterInstance = NULL;
    AudioEngine::errorCheck(it->second->getParameterByName(eventParameter.c_str(), parameter));
    //AudioEngine::errorCheck(parameterInstance->getValue(parameter));
}

// ParameterInstance is deprecated, using FMOD::Studio::EventInstance::setParameterByName
void AudioEngine::setEventParameter(const std::string& eventName, const std::string& parameterName, float value)
{
    auto it = implementation->events.find(eventName);
    if (it == implementation->events.end())
    {
        return;
    }
    //FMOD::Studio::ParameterInstance* pParameter = NULL;
    AudioEngine::errorCheck(it->second->setParameterByName(parameterName.c_str(), value));
    //CAudioEngine::ErrorCheck(pParameter->setValue(fValue));
}

void AudioEngine::setChannel3dPosition(int channelId, const glm::vec3& pos)
{
    auto it = implementation->channels.find(channelId);
    if (it == implementation->channels.end())
    {
        return;
    }

    FMOD_VECTOR position = vectorToFmod(pos);
    AudioEngine::errorCheck(it->second->set3DAttributes(&position, NULL));
}

void AudioEngine::setChannelVolume(int channelId, float volumedB)
{
    auto it = implementation->channels.find(channelId);
    if (it == implementation->channels.end())
    {
        return;
    }
    AudioEngine::errorCheck(it->second->setVolume(dBToVolume(volumedB)));
}

bool AudioEngine::isEventPlaying(const std::string& eventName) const
{
    auto it = implementation->events.find(eventName);
    if (it == implementation->events.end())
    {
        return false;
    }

    FMOD_STUDIO_PLAYBACK_STATE* state = 0;
    if (it->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING)
    {
        return true;
    }
    return false;
}

float AudioEngine::dBToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float AudioEngine::volumeTodB(float volume)
{
    return 20.0f * log10f(volume);
}

unsigned int AudioEngine::getSoundLengthInMS(const std::string& soundName)
{
    unsigned int out = 0;
    implementation->sounds[soundName]->getLength(&out, FMOD_TIMEUNIT_MS);
    return out;

}

FMOD_VECTOR AudioEngine::vectorToFmod(const glm::vec3& pos)
{
    FMOD_VECTOR vec;
    vec.x = pos.x;
    vec.y = pos.y;
    vec.z = pos.z;
    return vec;
}
