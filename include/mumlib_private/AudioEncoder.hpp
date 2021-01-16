#pragma once

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"
#include "mumlib_private/AudioResampler.hpp"

namespace mumlib {
    class AudioEncoder {
    public:
        //mark as non-copyable
        AudioEncoder(const AudioEncoder&) = delete;
        AudioEncoder& operator=(const AudioEncoder&) = delete;
        
        //ctor/dtor
        explicit AudioEncoder(uint32_t input_samplerate, uint32_t output_bitrate);
        ~AudioEncoder();

        int Process(const int16_t* pcmData, size_t pcmLength, uint8_t* encodedBuffer, size_t outputBufferSize);
        
        void Reset();

        uint32_t GetInputSamplerate();
        uint32_t GetOutputSamplerate();

        bool SetInputSamplerate(uint32_t samplerate);

        void SetBitrate(uint32_t bitrate);

    private:
        //encoder
        void createOpus();
        void createResampler();
        void destroyOpus();


    private:
        Logger logger = Logger("mumlib/AudioEncoder");

        OpusEncoder* _encoder = nullptr;

        std::unique_ptr<AudioResampler> _resampler;
        std::vector<int16_t> _resampler_buf;
        
        uint32_t _channels = 0;
        uint32_t _samplerate_input = 0;
        uint32_t _samplerate_output = 0;
    };
}
