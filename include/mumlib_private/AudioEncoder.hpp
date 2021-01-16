#pragma once

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"

namespace mumlib {
    class AudioEncoder {
    public:
        //mark as non-copyable
        AudioEncoder(const AudioEncoder&) = delete;
        AudioEncoder& operator=(const AudioEncoder&) = delete;
        
        //ctor/dtor
        explicit AudioEncoder(uint32_t encoding_bitrate);
        ~AudioEncoder();

        int EncoderProcess(const int16_t* pcmData, size_t pcmLength, uint8_t* encodedBuffer, size_t outputBufferSize);
        int EncoderProcess(const std::vector<int16_t>& input, uint8_t* encodedBuffer, size_t outputBufferSize);
        void EncoderReset();

    private:
        //encoder
        void encoderCreate(uint32_t sampleRate, uint32_t channels);
        void encoderDestroy();

        void encoderSetBitrate(uint32_t bitrate);
    private:
        Logger logger = Logger("mumlib/AudioEncoder");

        OpusEncoder* _encoder = nullptr;
    };
}
