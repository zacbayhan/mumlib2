//stdlib
#include <array>

//boost
#include <boost/format.hpp>

//mumlib
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/Audio.hpp"
#include "mumlib_private/Transport.hpp"

static boost::posix_time::seconds RESET_SEQUENCE_NUMBER_INTERVAL(5);

namespace mumlib {

    //
    // Ctor/Dtor
    //

    Audio::Audio(uint32_t bitrate) {
        encoderCreate(mumble_audio_samplerate, mumble_audio_channels);
        encoderSetBitrate(bitrate);
        EncoderReset();

        decoderCreate(mumble_audio_samplerate, mumble_audio_channels);
    }

    Audio::~Audio() {
        encoderDestroy();
        decoderDestroy();
    }

    //
    // Encoder
    //

    void Audio::encoderCreate(uint32_t samplerate, uint32_t channels)
    {
        encoderDestroy();

        int error = 0;
        _encoder = opus_encoder_create(samplerate, channels, OPUS_APPLICATION_VOIP, &error);
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize OPUS encoder: %s") % opus_strerror(error)).str());
        }
    }

    void Audio::encoderDestroy()
    {
        if (_encoder) {
            opus_encoder_destroy(_encoder);
            _encoder = nullptr;
        }
    }

    void mumlib::Audio::EncoderReset() {
        if (!_encoder) {
            throw AudioException("failed to reset encoder");
        }

        int status = opus_encoder_ctl(_encoder, OPUS_RESET_STATE, nullptr);
        if (status != OPUS_OK) {
            throw AudioException((boost::format("failed to reset encoder: %s") % opus_strerror(status)).str());
        }
    }

    void Audio::encoderSetBitrate(uint32_t bitrate)
    {
        if (!_encoder) {
            throw AudioException("failed to reset encoder");
        }

        int error = opus_encoder_ctl(_encoder, OPUS_SET_VBR(0));
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize variable bitrate: %s") % opus_strerror(error)).str());
        }

        error = opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(bitrate));
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize transmission bitrate to %d B/s: %s")
                % bitrate % opus_strerror(error)).str());
        }
    }

    int Audio::EncoderProcess(const int16_t* pcmData, size_t pcmLength, uint8_t* encodedBuffer, size_t outputBufferSize) {
        int outputSize = opus_encode(
            _encoder,
            pcmData,
            pcmLength,
            encodedBuffer,
            outputBufferSize
        );

        if (outputSize <= 0) {
            throw AudioException((boost::format("failed to encode %d B of PCM data: %s") % pcmLength % opus_strerror(outputSize)).str());
        }

        return outputSize;
    }

    int Audio::EncoderProcess(const std::vector<int16_t>& input, uint8_t* encodedBuffer, size_t outputBufferSize) {
        return EncoderProcess(input.data(), input.size(), encodedBuffer, outputBufferSize);
    }

    //
    // decoder
    //
    void Audio::decoderCreate(uint32_t samplerate, uint32_t channels)
    {
        decoderDestroy();

        int error = 0;
        _decoder = opus_decoder_create(samplerate, channels, &error);
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize OPUS decoder: %s") % opus_strerror(error)).str());
        }
    }

    void Audio::decoderDestroy()
    {
        if (_decoder) {
            opus_decoder_destroy(_decoder);
            _decoder = nullptr;
        }
    }

    int mumlib::Audio::decoderProcess(const std::vector<uint8_t>& input, int16_t* pcmBuffer, int pcmBufferSize) {
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