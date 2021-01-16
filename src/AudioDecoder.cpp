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

    AudioDecoder::AudioDecoder(uint32_t output_samplerate) {
        _samplerate_input = MUMBLE_AUDIO_SAMPLERATE;
        _samplerate_output = output_samplerate;
        _channels = MUMBLE_AUDIO_CHANNELS;

        createOpus();
        SetOutputSamplerate(output_samplerate);

        Reset();
    }

    AudioDecoder::~AudioDecoder() {
        destroyOpus();
    }

    void AudioDecoder::Reset()
    {
        if (!_decoder) {
            throw AudioException("failed to reset encoder");
        }

        int status = opus_decoder_ctl(_decoder, OPUS_RESET_STATE, nullptr);
        if (status != OPUS_OK) {
            throw AudioException((boost::format("failed to reset encoder: %s") % opus_strerror(status)).str());
        }

        if (_resampler) {
            _resampler->Reset();
        }
    }

    uint32_t AudioDecoder::GetInputSamplerate()
    {
        return _samplerate_input;
    }

    uint32_t AudioDecoder::GetOutputSamplerate()
    {
        return _samplerate_output;
    }

    bool AudioDecoder::SetOutputSamplerate(uint32_t samplerate)
    {
        _samplerate_output = samplerate;
        _decoder_buf.resize(_samplerate_output * MUMBLE_AUDIO_CHANNELS * MUMBLE_OPUS_MAXLENGTH / 1000);
        createResampler();
        return true;
    }
    
    void AudioDecoder::createOpus()
    {
        destroyOpus();

        int error = 0;
        _decoder = opus_decoder_create(_samplerate_input, _channels, &error);
        if (error != OPUS_OK) {
            throw AudioException((boost::format("failed to initialize OPUS decoder: %s") % opus_strerror(error)).str());
        }
    }

    void AudioDecoder::createResampler()
    {
        _resampler.reset();
        if (_samplerate_input != _samplerate_output) {
            _resampler = std::make_unique<AudioResampler>(_channels, _samplerate_input, _samplerate_output, MUMBLE_RESAMPLER_QUALITY);
            _resampler_buf.resize(_samplerate_output * _channels * MUMBLE_OPUS_MAXLENGTH / 1000);
        }
    }

    void AudioDecoder::destroyOpus()
    {
        if (_decoder) {
            opus_decoder_destroy(_decoder);
            _decoder = nullptr;
        }
    }

    std::pair<int16_t*, size_t> AudioDecoder::Process(const std::vector<uint8_t>& input) {
        //decode
        int outputSize = opus_decode(
            _decoder,
            input.data(),
            input.size(),
            _decoder_buf.data(),
            _decoder_buf.size(),
            0
        );

        if (outputSize <= 0) {
            throw AudioException((boost::format("failed to decode %d B of OPUS data: %s") % input.size() % opus_strerror(outputSize)).str());
        }

        //resample
        if (_resampler) {
            int resampled_samples = _resampler->Process(_decoder_buf.data(), outputSize, _resampler_buf.data(), _resampler_buf.size());
            memcpy(_decoder_buf.data(), _resampler_buf.data(), resampled_samples*sizeof(int16_t));
            outputSize = resampled_samples;
        }

        if (outputSize <= 0) {
            throw AudioException("failed to resample");
        }

        return std::make_pair(_decoder_buf.data(), outputSize);
    }
}
