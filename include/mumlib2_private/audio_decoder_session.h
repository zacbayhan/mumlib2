// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib2/logger.h"
#include "mumlib2_private/audio_packet.h"

namespace mumlib2 {
    class AudioDecoderSession {
    public:
        //mark as non-copyable
        AudioDecoderSession(const AudioDecoderSession&) = delete;
        AudioDecoderSession& operator=(const AudioDecoderSession&) = delete;
        
        //ctor/dtor
        explicit AudioDecoderSession(int32_t session_id, uint32_t channels);
        ~AudioDecoderSession();

        std::pair<const int16_t*, size_t> Process(const AudioPacket& packet);

        std::chrono::time_point<std::chrono::steady_clock> GetLastTimepoint();

    private:
        void opusCreate();
        size_t opusDecode(const uint8_t* in_data, size_t in_len);
        void opusDestroy();
        void opusResize();

        void reset();

    private:
        Logger logger = Logger("mumlib/AudioDecoderSession");

        OpusDecoder* _opus = nullptr;
        std::vector<int16_t> _opus_output_buf;

        uint32_t _channels = 0;
        int32_t _session_id;

        std::chrono::time_point<std::chrono::steady_clock> _timepoint_last;
    };
}
