// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <chrono>
#include <cstdint>
#include <map>
#include <utility>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib2/logger.h"
#include "mumlib2_private/audio_decoder_session.h"
#include "mumlib2_private/audio_packet.h"

namespace mumlib2 {
    class AudioDecoder {
    public:
        //mark as non-copyable
        AudioDecoder(const AudioDecoder&) = delete;
        AudioDecoder& operator=(const AudioDecoder&) = delete;
        
        //ctor/dtor
        explicit AudioDecoder(uint32_t channels);
        ~AudioDecoder();

        std::pair<const int16_t*, size_t> Process(const AudioPacket& packet);

    private:
        Logger _logger = Logger("mumlib/AudioDecoder");

        uint32_t _channels = 0;

        const std::chrono::seconds _timeout_inactivity = std::chrono::seconds(300);

        std::map<int32_t, std::unique_ptr<AudioDecoderSession>> _sessions;
    };
}
