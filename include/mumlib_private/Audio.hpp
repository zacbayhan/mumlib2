#pragma once

//stdlib
#include <chrono>
#include <map>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib.hpp"
#include "mumlib/Constants.hpp"
#include "mumlib/Logger.hpp"

namespace mumlib {
    class Audio {
    public:
        //mark as non-copyable
        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;
        
        //ctor/dtor
        explicit Audio(uint32_t encoding_bitrate);
        ~Audio();

        int decoderProcess(const std::vector<uint8_t>& input, int16_t* pcmBuffer, int pcmBufferSize);
        
        int EncoderProcess(const int16_t* pcmData, size_t pcmLength, uint8_t* encodedBuffer, size_t outputBufferSize);
        int EncoderProcess(const std::vector<int16_t>& input, uint8_t* encodedBuffer, size_t outputBufferSize);
        void EncoderReset();

    private:
        //encoder
        void encoderCreate(uint32_t sampleRate, uint32_t channels);
        void encoderDestroy();

        void encoderSetBitrate(uint32_t bitrate);
       
        //decoder
        void decoderCreate(uint32_t sampleRate, uint32_t channels);
        void decoderDestroy();
    private:
        Logger logger = Logger("Mumlib.audio");

        OpusDecoder* _decoder = nullptr;
        OpusEncoder* _encoder = nullptr;
    };
}
