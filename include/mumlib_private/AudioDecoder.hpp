#pragma once

//stdlib
#include <chrono>
#include <cstdint>
#include <map>
#include <utility>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"
#include "mumlib_private/AudioDecoderSession.hpp"
#include "mumlib_private/AudioPacket.hpp"
#include "mumlib_private/AudioResampler.hpp"

namespace mumlib {
    class AudioDecoder {
    public:
        //mark as non-copyable
        AudioDecoder(const AudioDecoder&) = delete;
        AudioDecoder& operator=(const AudioDecoder&) = delete;
        
        //ctor/dtor
        explicit AudioDecoder(uint32_t samplerate_input, uint32_t samplerate_output, uint32_t channels);
        ~AudioDecoder();

        uint32_t GetInputSamplerate();
        uint32_t GetOutputSamplerate();

        void SetInputSamplerate(uint32_t samplerate);
        void SetOutputSamplerate(uint32_t samplerate);

        std::pair<const int16_t*, size_t> Process(const AudioPacket& packet);

    private:
        Logger _logger = Logger("mumlib/AudioDecoder");

        uint32_t _channels = 0;
        uint32_t _samplerate_input = 0;
        uint32_t _samplerate_output = 0;

        const std::chrono::seconds _timeout_inactivity = std::chrono::seconds(300);

        std::map<int32_t, std::unique_ptr<AudioDecoderSession>> _sessions;
    };
}
