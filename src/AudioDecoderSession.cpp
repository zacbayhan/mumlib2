//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioDecoderSession.hpp"

namespace mumlib {
	AudioDecoderSession::AudioDecoderSession(int32_t session_id, uint32_t channels)
	{
		_session_id = session_id;
		_channels = channels;

		opusCreate();
		opusResize();
	}

	AudioDecoderSession::~AudioDecoderSession()
	{
		opusDestroy();
	}

	std::chrono::time_point<std::chrono::steady_clock> AudioDecoderSession::GetLastTimepoint()
	{
		return _timepoint_last;
	}
	
	void AudioDecoderSession::opusCreate()
	{
		opusDestroy();

		int error = 0;
		_opus = opus_decoder_create(MUMBLE_AUDIO_SAMPLERATE, _channels, &error);
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
		size_t target_size = MUMBLE_AUDIO_SAMPLERATE * MUMBLE_AUDIO_CHANNELS * MUMBLE_OPUS_MAXLENGTH / 1000;
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
		}

		//reset
		if (packet.GetAudioLastFlag()) {
			reset();
		}

		_timepoint_last = std::chrono::steady_clock::now();

		return std::make_pair(result_data, result_size);
	}
}
