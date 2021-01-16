#pragma once

//stdlib
#include <chrono>
#include <cstdint>
#include <memory>
#include <vector>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib/Logger.hpp"
#include "mumlib_private/AudioPacket.hpp"
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

        std::vector<uint8_t> Encode(const int16_t* pcmData, size_t pcmLength, uint32_t target);
        
        uint32_t GetInputSamplerate();
        uint32_t GetOutputSamplerate();

        bool SetInputSamplerate(uint32_t samplerate);

        void SetBitrate(uint32_t bitrate);

    private:
        void reset();

        void createOpus();
        void createResampler();
        void destroyOpus();


    private:
        Logger logger = Logger("mumlib/AudioEncoder");

        OpusEncoder* _encoder = nullptr;
        std::vector<uint8_t> _encoder_buf;

        std::unique_ptr<AudioResampler> _resampler;
        std::vector<int16_t> _resampler_buf;
        
        uint32_t _channels = 0;

        uint32_t _samplerate_input = 0;
        uint32_t _samplerate_output = 0;

        std::chrono::time_point<std::chrono::high_resolution_clock> _sequence_timestemp;
        uint32_t _sequence_number = 0;

    private:
        static constexpr std::chrono::seconds _sequence_reset_interval = std::chrono::seconds(5);
    };
}
