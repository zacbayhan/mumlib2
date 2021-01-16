#pragma once

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"

namespace mumlib {
    class AudioDecoder {
    public:
        //mark as non-copyable
        AudioDecoder(const AudioDecoder&) = delete;
        AudioDecoder& operator=(const AudioDecoder&) = delete;
        
        //ctor/dtor
        explicit AudioDecoder();
        ~AudioDecoder();

        int decoderProcess(const std::vector<uint8_t>& input, int16_t* pcmBuffer, int pcmBufferSize);

    private:
        //decoder
        void decoderCreate(uint32_t sampleRate, uint32_t channels);
        void decoderDestroy();
    private:
        Logger logger = Logger("mumlib/AudioDecoder");

        OpusDecoder* _decoder = nullptr;
    };
}
