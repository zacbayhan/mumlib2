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

    AudioDecoder::AudioDecoder(uint32_t channels)
    {
        _channels = channels;
    }

    AudioDecoder::~AudioDecoder() {
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
            _sessions.emplace(session_id, std::make_unique<AudioDecoderSession>(session_id, _channels));
        }

        return _sessions[session_id]->Process(packet);
    }
}
