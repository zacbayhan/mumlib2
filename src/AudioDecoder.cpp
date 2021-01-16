//stdlib
#include <array>

//boost
#include <boost/format.hpp>

//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioDecoder.hpp"

namespace mumlib {

    //
    // Ctor/Dtor
    //

    AudioDecoder::AudioDecoder() {
        decoderCreate(MUMBLE_AUDIO_SAMPLERATE, MUMBLE_AUDIO_CHANNELS);
    }

    AudioDecoder::~AudioDecoder() {
        decoderDestroy();
    }

    //
    // decoder
    //
    void AudioDecoder::decoderCreate(uint32_t samplerate, uint32_t channels)
    {
        decoderDestroy();

        int error = 0;
        _decoder = opus_decoder_create(samplerate, channels, &error);
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize OPUS decoder: %s") % opus_strerror(error)).str());
        }
    }

    void AudioDecoder::decoderDestroy()
    {
        if (_decoder) {
            opus_decoder_destroy(_decoder);
            _decoder = nullptr;
        }
    }

    int AudioDecoder::decoderProcess(const std::vector<uint8_t>& input, int16_t* pcmBuffer, int pcmBufferSize) {
        int outputSize = opus_decode(
            _decoder,
            input.data(),
            input.size(),
            pcmBuffer,
            pcmBufferSize,
            0
        );

        if (outputSize <= 0) {
            throw AudioException((boost::format("failed to decode %d B of OPUS data: %s") % input.size() % opus_strerror(outputSize)).str());
        }

        return outputSize;
    }
}
