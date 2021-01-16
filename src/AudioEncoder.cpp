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

    AudioEncoder::AudioEncoder(uint32_t input_samplerate, uint32_t output_bitrate) {
        _samplerate_input = input_samplerate;
        _samplerate_output = MUMBLE_AUDIO_SAMPLERATE;
        _channels = MUMBLE_AUDIO_CHANNELS;


        createOpus();
        createResampler();

        SetBitrate(output_bitrate);

        Reset();
    }

    AudioEncoder::~AudioEncoder() {
        destroyOpus();
    }

    void AudioEncoder::createOpus()
    {
        destroyOpus();

        int error = 0;
        _encoder = opus_encoder_create(_samplerate_output, _channels, OPUS_APPLICATION_VOIP, &error);
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize OPUS encoder: %s") % opus_strerror(error)).str());
        }
    }

    void AudioEncoder::createResampler()
    {
        _resampler.reset();
        if (_samplerate_input != _samplerate_output) {
            _resampler = std::make_unique<AudioResampler>(_channels, _samplerate_input, _samplerate_output, MUMBLE_RESAMPLER_QUALITY);
            _resampler_buf.resize(_samplerate_output * _channels * MUMBLE_OPUS_MAXLENGTH / 1000);
        }
    }

    void AudioEncoder::destroyOpus()
    {
        if (_encoder) {
            opus_encoder_destroy(_encoder);
            _encoder = nullptr;
        }
    }

    void AudioEncoder::Reset() {
        if (!_encoder) {
            throw AudioException("failed to reset encoder");
        }

        int status = opus_encoder_ctl(_encoder, OPUS_RESET_STATE, nullptr);
        if (status != OPUS_OK) {
            throw AudioException((boost::format("failed to reset encoder: %s") % opus_strerror(status)).str());
        }

        if (_resampler) {
            _resampler->Reset();
        }
    }

    uint32_t AudioEncoder::GetInputSamplerate()
    {
        return _samplerate_input;
    }

    uint32_t AudioEncoder::GetOutputSamplerate()
    {
        return _samplerate_output;
    }

    bool AudioEncoder::SetInputSamplerate(uint32_t samplerate)
    {
        _samplerate_input = samplerate;
        createResampler();
        return true;
    }

    void AudioEncoder::SetBitrate(uint32_t bitrate)
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

    int AudioEncoder::Process(const int16_t* pcmData, size_t pcmLength, uint8_t* encodedBuffer, size_t outputBufferSize) {

        const int16_t* in_data = pcmData;
        size_t in_len = pcmLength;

        if (_resampler) {
            in_len = _resampler->Process(pcmData, pcmLength, _resampler_buf.data(), _resampler_buf.size());
            in_data = _resampler_buf.data();
        }

        int outputSize = opus_encode(
            _encoder,
            in_data,
            in_len,
            encodedBuffer,
            outputBufferSize
        );

        if (outputSize <= 0) {
            throw AudioException((boost::format("failed to encode %d B of PCM data: %s") % pcmLength % opus_strerror(outputSize)).str());
        }

        return outputSize;
    }
}