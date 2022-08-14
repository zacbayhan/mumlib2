// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <array>
#include <cstdint>
#include <vector>

//mumlib
#include "mumlib2/enums.h"

namespace mumlib2 {

	class AudioPacket {
	public:
		static AudioPacket Decode(const uint8_t* buffer, size_t length, size_t pos);
		
		static AudioPacket CreateAudioOpusPacket(uint8_t target, int64_t sequence_number, const uint8_t* payload, size_t payload_len, bool is_last);
		static AudioPacket CreatePingPacket(int64_t timestamp);

		~AudioPacket() = default;

		//
		// Encode
		//
		std::vector<uint8_t> Encode();

		//
		// Getters
		//
		uint8_t GetHeaderTarget() const;
		AudioPacketType GetHeaderType() const;

		const std::vector<uint8_t>& GetAudioPayload() const;
		int64_t GetAudioSessionId() const;
		int64_t GetAudioSequenceNumber() const;
		bool GetAudioLastFlag() const;
		const std::array<float, 3>& GetAudioPosition() const;

		int64_t GetPingTimestamp() const;

	private:
		AudioPacket() = default;

		void parse_header(const uint8_t* buffer, size_t length, size_t pos);
		void parse_audio(const uint8_t* buffer, size_t length, size_t pos);
		void parse_ping(const uint8_t* buffer, size_t length, size_t pos);

	private:
		//header fields
		AudioPacketType _header_type;
		uint8_t _header_target = 0;

		//audio fields
		int64_t _audio_sessionid  = 0;
		int64_t _audio_sequencenum = 0;
		bool _audio_last = false;
		std::vector<uint8_t> _audio_payload;
		std::array<float, 3> _audio_position{};

		//ping fields
		int64_t _ping_timestamp = 0;

	private:
		static constexpr uint8_t _header_type_mask   = 0b11100000;
		static constexpr uint8_t _header_target_mask = 0b00011111;

		static constexpr uint16_t _audio_opus_last_mask   = 0x2000;
		static constexpr uint16_t _audio_opus_length_mask = 0x1FFF;
	};
}