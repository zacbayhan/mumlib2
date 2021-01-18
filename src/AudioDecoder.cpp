//stdlib
#include <array>

//boost
#include <boost/format.hpp>

//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioDecoder.hpp"

namespace mumlib {

    //
    // Ctor/Dtor
    //

    AudioDecoder::AudioDecoder(uint32_t samplerate_input, uint32_t samplerate_output, uint32_t channels)
    {
        _samplerate_input = samplerate_input;
        _samplerate_output = samplerate_output;
        _channels = channels;
    }

    AudioDecoder::~AudioDecoder() {
    }


    uint32_t AudioDecoder::GetInputSamplerate()
    {
        return _samplerate_input;
    }

    uint32_t AudioDecoder::GetOutputSamplerate()
    {
        return _samplerate_output;
    }

    void AudioDecoder::SetInputSamplerate(uint32_t samplerate)
    {
        _samplerate_input = samplerate;
        for (auto& session : _sessions) {
            session.second->SetInputSamplerate(samplerate);
        }
    }

    void AudioDecoder::SetOutputSamplerate(uint32_t samplerate)
    {
        _samplerate_output = samplerate;
        for (auto& session : _sessions) {
            session.second->SetOutputSamplerate(samplerate);
        }
    }

    std::pair<const int16_t*, size_t> AudioDecoder::Process(const AudioPacket& packet)
    {
        //cleanup
        auto current_time = std::chrono::steady_clock::now();
        for (auto it = _sessions.begin(); it != _sessions.end();)
        {
            if ((current_time - it->second->GetLastTimepoint()) > _timeout_inactivity) {
                _sessions.erase(it++);
            }
            else {
                ++it;
            }
        }

        //process
        auto session_id = packet.GetAudioSessionId();
        if (!_sessions.contains(session_id)) {
            _sessions.emplace(session_id, std::make_unique<AudioDecoderSession>(session_id, _samplerate_input, _samplerate_output, _channels));
        }

        return _sessions[session_id]->Process(packet);
    }
}
