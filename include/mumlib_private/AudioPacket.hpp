#pragma once

//stdlib
#include <array>
#include <cstdint>
#include <vector>

//mumlib
#include "mumlib/Enums.hpp"

namespace mumlib {

	class AudioPacket {
	public:
		explicit AudioPacket(const uint8_t* buffer, size_t length);
		~AudioPacket() = default;

		//
		// Getters
		//
		const std::vector<uint8_t>& GetAudioPayload();
		int64_t GetAudioSessionId();
		int64_t GetAudioSequenceNumber();
		bool GetAudioLastFlag();

		uint8_t GetTarget();
		AudioPacketType GetType();

	private:
		void parse_audio_in(const uint8_t* buffer, size_t length, size_t pos);

	private:
		AudioPacketType _type;
		uint8_t _target = 0;

		int64_t _session  = 0;
		int64_t _sequence = 0;

		bool _audio_last = false;
		std::vector<uint8_t> _payload;
		std::array<float, 3> _position{};

	private:
		static constexpr uint8_t packet_type_mask   = 0b11100000;
		static constexpr uint8_t packet_target_mask = 0b00011111;

		static constexpr uint8_t _audio_opus_last_mask = 0x2000;
		static constexpr uint8_t _audio_opus_length_mask = 0x1FFF;
	};
}