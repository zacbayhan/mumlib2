//mumlib
#include "mumlib_private/AudioResampler.hpp"

namespace mumlib {
	AudioResampler::AudioResampler(int32_t channels, uint32_t samplerate_in, uint32_t samplerate_out, int quality)
	{
		int error = 0;
		_state = speex_resampler_init(channels, samplerate_in, samplerate_out, quality, &error);
	}

	AudioResampler::~AudioResampler() {
		if (_state) {
			speex_resampler_destroy(_state);
		}
	}

	uint32_t AudioResampler::Process(const int16_t* in_samples, uint32_t in_len, int16_t* out_samples, uint32_t out_len)
	{
		if (!_state) {
			return 0;
		}

		uint32_t in_len_tmp = in_len;
		uint32_t out_len_tmp = out_len;
		speex_resampler_process_interleaved_int(_state, in_samples, &in_len_tmp, out_samples, &out_len_tmp);

		return out_len_tmp;
	}

	void AudioResampler::Reset()
	{
		if (!_state) {
			return;
		}

		speex_resampler_reset_mem(_state);
	}
}