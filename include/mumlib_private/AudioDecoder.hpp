#pragma once

//stdlib
#include <cstdint>
#include <utility>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"
#include "mumlib_private/AudioResampler.hpp"

namespace mumlib {
    class AudioDecoder {
    public:
        //mark as non-copyable
        AudioDecoder(const AudioDecoder&) = delete;
        AudioDecoder& operator=(const AudioDecoder&) = delete;
        
        //ctor/dtor
        explicit AudioDecoder(uint32_t output_samplerate);
        ~AudioDecoder();

        void Reset();

        uint32_t GetInputSamplerate();
        uint32_t GetOutputSamplerate();

        bool SetOutputSamplerate(uint32_t samplerate);

        std::pair<int16_t*, size_t> Process(const std::vector<uint8_t>& input);

    private:
        //decoder
        void createOpus();
        void createResampler();
        void destroyOpus();

    private:
        Logger logger = Logger("mumlib/AudioDecoder");

        OpusDecoder* _decoder = nullptr;
        std::vector<int16_t> _decoder_buf;

        std::unique_ptr<AudioResampler> _resampler;
        std::vector<int16_t> _resampler_buf;

        uint32_t _channels = 0;
        uint32_t _samplerate_input = 0;
        uint32_t _samplerate_output = 0;
    };
}
