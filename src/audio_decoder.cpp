// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

//stdlib
#include <array>
#include <chrono>

//mumlib
#include "mumlib2/constants.h"
#include "mumlib2/exceptions.h"
#include "mumlib2_private/audio_decoder.h"

namespace mumlib2 {

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
