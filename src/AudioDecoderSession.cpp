//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioDecoderSession.hpp"

namespace mumlib {
	AudioDecoderSession::AudioDecoderSession(int32_t session_id, uint32_t samplerate_input, uint32_t samplerate_output, uint32_t channels)
	{
		_session_id = session_id;
		_samplerate_input = samplerate_input;
		_samplerate_output = samplerate_output;
		_channels = channels;

		opusCreate();
		opusResize();
		resamplerCreate();
	}

	AudioDecoderSession::~AudioDecoderSession()
	{
		opusDestroy();
	}

	bool AudioDecoderSession::SetInputSamplerate(uint32_t samplerate)
	{
		_samplerate_input = samplerate;
		opusCreate();
		resamplerCreate();
		return true;
	}

	bool AudioDecoderSession::SetOutputSamplerate(uint32_t samplerate)
	{
		_samplerate_output = samplerate;
		opusResize();
		resamplerCreate();
		return true;
	}
	
	void AudioDecoderSession::opusCreate()
	{
		opusDestroy();

		int error = 0;
		_opus = opus_decoder_create(_samplerate_input, _channels, &error);
		if (error != OPUS_OK) {
			throw AudioDecoderException("opusCreate-> failed to init decoder");
		}
	}

	size_t AudioDecoderSession::opusDecode(const uint8_t* in_data, size_t in_len)
	{
		if (!_opus) {
			throw AudioDecoderException("opusDecode: no decoder");
		}

		return opus_decode(_opus, in_data, in_len, _opus_output_buf.data(), _opus_output_buf.size(), 0);
	}

	void AudioDecoderSession::opusDestroy()
	{
		if (_opus) {
			opus_decoder_destroy(_opus);
			_opus = nullptr;
		}
	}

	void AudioDecoderSession::opusResize()
	{
		size_t target_size = _samplerate_output * MUMBLE_AUDIO_CHANNELS * MUMBLE_OPUS_MAXLENGTH / 1000;
		if (_opus_output_buf.size() != target_size) {
			_opus_output_buf.resize(target_size);
		};
	}

	void AudioDecoderSession::reset()
	{
		if (!_opus) {
			throw AudioDecoderException("failed to reset encoder 1");
		}

		int status = opus_decoder_ctl(_opus, OPUS_RESET_STATE, nullptr);
		if (status != OPUS_OK) {
			throw AudioDecoderException("failed to reset encoder: 2");
		}

		if (_resampler) {
			_resampler->Reset();
		}
	}

	void AudioDecoderSession::resamplerCreate()
	{
		_resampler.reset();
		if (_samplerate_input != _samplerate_output) {
			_resampler = std::make_unique<AudioResampler>(_channels, _samplerate_input, _samplerate_output, MUMBLE_RESAMPLER_QUALITY);
			_resampler_buf.resize(_samplerate_output * _channels * MUMBLE_OPUS_MAXLENGTH / 1000);
		}
	}
	std::pair<const int16_t*, size_t> AudioDecoderSession::Process(const AudioPacket& packet)
	{
		int16_t* result_data = nullptr;
		size_t result_size = 0;

		auto& payload = packet.GetAudioPayload();
		if (payload.size()) {
			result_size = opusDecode(payload.data(), payload.size());
			result_data = _opus_output_buf.data();

			if (result_size <= 0) {
				throw AudioDecoderException("failed to decode opus data");
			}

			//resample
			if (_resampler) {
				result_size = _resampler->Process(_opus_output_buf.data(), result_size, _resampler_buf.data(), _resampler_buf.size());
				result_data = _resampler_buf.data();
			}
		}

		//reset
		if (packet.GetAudioLastFlag()) {
			reset();
		}

		return std::make_pair(result_data, result_size);
	}
}
