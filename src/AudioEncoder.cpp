//stdlib
#include <array>

//boost
#include <boost/format.hpp>

//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioEncoder.hpp"

namespace mumlib {

    //
    // Ctor/Dtor
    //

    AudioEncoder::AudioEncoder(uint32_t output_bitrate) {
        _channels = MUMBLE_AUDIO_CHANNELS;

        createOpus();

        SetBitrate(output_bitrate);

        reset();
    }

    AudioEncoder::~AudioEncoder() {
        destroyOpus();
    }

    void AudioEncoder::createOpus()
    {
        destroyOpus();

        int error = 0;
        _encoder = opus_encoder_create(MUMBLE_AUDIO_SAMPLERATE, _channels, OPUS_APPLICATION_VOIP, &error);
        if (error != OPUS_OK) {
            throw AudioEncoderException((boost::format("failed to initialize OPUS encoder: %s") % opus_strerror(error)).str());
        }
    }

    void AudioEncoder::destroyOpus()
    {
        if (_encoder) {
            opus_encoder_destroy(_encoder);
            _encoder = nullptr;
        }
    }

    void AudioEncoder::reset() {
        if (!_encoder) {
            throw AudioEncoderException("failed to reset encoder");
        }

        int status = opus_encoder_ctl(_encoder, OPUS_RESET_STATE, nullptr);
        if (status != OPUS_OK) {
            throw AudioEncoderException((boost::format("failed to reset encoder: %s") % opus_strerror(status)).str());
        }

        _sequence_number = 0;
    }

    void AudioEncoder::SetBitrate(uint32_t bitrate)
    {
        _encoder_buf.resize(bitrate * MUMBLE_OPUS_MAXLENGTH / 1000);

        if (!_encoder) {
            throw AudioEncoderException("failed to reset encoder");
        }

        int error = opus_encoder_ctl(_encoder, OPUS_SET_VBR(0));
        if (error != OPUS_OK) {
            throw AudioEncoderException((boost::format("failed to initialize variable bitrate: %s") % opus_strerror(error)).str());
        }

        error = opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(bitrate));
        if (error != OPUS_OK) {
            throw AudioEncoderException((boost::format("failed to initialize transmission bitrate to %d B/s: %s")
                % bitrate % opus_strerror(error)).str());
        }
    }

    std::vector<uint8_t> AudioEncoder::Encode(const int16_t* pcmData, size_t pcmLength, uint32_t target) {
        const int16_t* in_data = pcmData;
        int in_len = pcmLength;

        int out_len = 0;

        //check interval and reset encoder
        auto interval = std::chrono::high_resolution_clock::now() - _sequence_timestemp;
        if (interval > _sequence_reset_interval) {
            reset();
        }
        
        //resample and encode
        if (pcmData && pcmLength) {
            out_len = opus_encode(
                _encoder,
                in_data,
                in_len,
                _encoder_buf.data(),
                _encoder_buf.size()
            );

            if (out_len <= 0) {
                throw AudioEncoderException((boost::format("failed to encode %d B of PCM data: %s") % in_len % opus_strerror(out_len)).str());
            }
        }

        //create audiopacket
        auto encoded = AudioPacket::CreateAudioOpusPacket(
            target,
            _sequence_number,
            _encoder_buf.data(),
            out_len,
            out_len == 0).Encode();

        //update timestamp and sequence
        if (out_len > 0) {
            //1 per 10ms
            _sequence_number += 100 * in_len / MUMBLE_AUDIO_SAMPLERATE;
        }
        else {
            reset();
        }

        _sequence_timestemp = std::chrono::steady_clock::now();

        return encoded;
    }
}