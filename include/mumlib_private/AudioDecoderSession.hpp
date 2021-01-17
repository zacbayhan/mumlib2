#pragma once

//stdlib
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
    class AudioDecoderSession {
    public:
        //mark as non-copyable
        AudioDecoderSession(const AudioDecoderSession&) = delete;
        AudioDecoderSession& operator=(const AudioDecoderSession&) = delete;
        
        //ctor/dtor
        explicit AudioDecoderSession(int32_t session_id, uint32_t samplerate_input, uint32_t samplerate_output, uint32_t channels);
        ~AudioDecoderSession();

        std::pair<const int16_t*, size_t> Process(const AudioPacket& packet);

        bool SetInputSamplerate(uint32_t samplerate);
        bool SetOutputSamplerate(uint32_t samplerate);

    private:
        void opusCreate();
        size_t opusDecode(const uint8_t* in_data, size_t in_len);
        void opusDestroy();
        void opusResize();

        void reset();
        void resamplerCreate();

    private:
        Logger logger = Logger("mumlib/AudioDecoderSession");

        OpusDecoder* _opus = nullptr;
        std::vector<int16_t> _opus_output_buf;

        std::unique_ptr<AudioResampler> _resampler;
        std::vector<int16_t> _resampler_buf;

        uint32_t _channels = 0;
        uint32_t _samplerate_input = 0;
        uint32_t _samplerate_output = 0;
        int32_t _session_id;
    };
}
