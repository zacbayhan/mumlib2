#pragma once

//stdlib 
#include <cstdint>

//speexdsp
#include <speex/speex_resampler.h>

//mumlib

namespace mumlib {
	class AudioResampler {
	public:
		AudioResampler(int32_t channels, uint32_t samplerate_in, uint32_t samplerate_out, int quality);
		~AudioResampler();

		uint32_t Process(const int16_t* in_samples, uint32_t in_len, int16_t* out_samples, uint32_t out_len);

		void Reset();

	private:
		SpeexResamplerState* _state;

	};
}