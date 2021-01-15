#include "mumlib/Exceptions.hpp"
#include "mumlib_private/AudioPacket.hpp"
#include "mumlib_private/VarInt.hpp"

namespace mumlib {

    //
    // Ctor
    //
    AudioPacket AudioPacket::Decode(const uint8_t* buffer, size_t length,size_t pos)
    {
        AudioPacket packet;
        packet.parse_header(buffer, length, pos);

        switch (packet.GetHeaderType()) {
            case AudioPacketType::CeltAplha:
            case AudioPacketType::Speex:
            case AudioPacketType::CeltBeta:
            case AudioPacketType::Opus:
                packet.parse_audio(buffer, length, pos+1);
                break;
            case AudioPacketType::Ping:
                packet.parse_ping(buffer, length, pos + 1);
                break;
            default:
                break;
        }
        return packet;
    }

    AudioPacket AudioPacket::CreateAudioOpusPacket(uint8_t target, int64_t sequence_number, const uint8_t* payload, size_t payload_len, bool is_last)
    {
        AudioPacket packet;
        packet._header_target = target;
        packet._header_type = AudioPacketType::Opus;

        packet._audio_last = is_last;
        packet._audio_sequencenum = sequence_number;
        packet._audio_payload = std::vector<uint8_t>(payload, payload + payload_len);
        return packet;
    }

    AudioPacket AudioPacket::CreatePingPacket(int64_t timestamp)
    {
        AudioPacket packet;
        packet._header_target = 0;
        packet._header_type = AudioPacketType::Ping;
        packet._ping_timestamp = timestamp;
        return packet;
    }

    //
    // Encode
    //
    std::vector<uint8_t> AudioPacket::Encode()
    {
        std::vector<uint8_t> result;
        result.push_back((_header_target & _header_target_mask) | (static_cast<uint8_t>(_header_type) & _header_type_mask));

        if (GetHeaderType() == AudioPacketType::Ping) {
            auto timestamp = VarInt(_ping_timestamp).Encode();
            result.insert(result.end(), timestamp.begin(), timestamp.end());
        }
        else if (GetHeaderType() == AudioPacketType::Opus) {
            //sequence number
            auto sequence_num = VarInt(_audio_sequencenum).Encode();
            result.insert(result.end(), sequence_num.begin(), sequence_num.end());

            //opus length
            uint16_t len = _audio_payload.size();
            if (GetAudioLastFlag()) {
                len |= _audio_opus_last_mask;
            }
            auto len_encoded = VarInt(len).Encode();
            result.insert(result.end(), len_encoded.begin(), len_encoded.end());

            //opus payload
            result.insert(result.end(), _audio_payload.begin(), _audio_payload.end());

            //TODO: position data
        }
        else {
            throw AudioPacketException("unsupported type");
        }

        return result;
    }

    //
    // Getters
    //    
    uint8_t AudioPacket::GetHeaderTarget()
    {
        return _header_target;
    }

    AudioPacketType AudioPacket::GetHeaderType()
    {
        return _header_type;
    }

    const std::vector<uint8_t>& AudioPacket::GetAudioPayload()
    {
        return _audio_payload;
    }

    int64_t AudioPacket::GetAudioSessionId()
    {
        return _audio_sessionid;
    }

    int64_t AudioPacket::GetAudioSequenceNumber()
    {
        return _audio_sequencenum;
    }

    bool AudioPacket::GetAudioLastFlag()
    {
        return _audio_last;
    }

    const std::array<float, 3>& AudioPacket::GetAudioPosition()
    {
        return _audio_position;
    }

    int64_t AudioPacket::GetPingTimestamp()
    {
        return _ping_timestamp;
    }

    //
    // Parser
    //

    void AudioPacket::parse_header(const uint8_t* buffer, size_t length, size_t pos)
    {
        _header_type = static_cast<AudioPacketType>(buffer[pos] & _header_type_mask);
        _header_target = buffer[pos] & _header_target_mask;
    }

    void AudioPacket::parse_audio(const uint8_t* buffer, size_t length, size_t pos)
    {
        //session ID
        VarInt varint_sessionid(&buffer[pos]);
        _audio_sessionid = varint_sessionid.Value();
        pos += varint_sessionid.Size();

        //sequence number
        VarInt varint_sequencenumber(&buffer[pos]);
        _audio_sequencenum = varint_sequencenumber.Value();
        pos += varint_sequencenumber.Size();

        //opus
        if (GetHeaderType() == AudioPacketType::Opus) {
            //parse header
            VarInt varint_sequencenumber(&buffer[pos]);
            int64_t header = varint_sequencenumber.Value();
            pos += varint_sequencenumber.Size();

            //parse last message bit
            _audio_last = (header & _audio_opus_last_mask) == _audio_opus_last_mask;

            //copy buffer
            auto opus_length = header & _audio_opus_length_mask;
            _audio_payload = std::vector<uint8_t>(buffer + pos, buffer + pos + opus_length);

            //increment pos
            pos += opus_length;
        }
        else {
            //TODO: support other voice types other than opus
            return;
        }

        //parse position data
        if (pos < length) {
            for (size_t i = 0; i < _audio_position.size(); i++) {
                _audio_position[i] = reinterpret_cast<const float*>(buffer + pos)[i];
            }
        }

        //check that we are not overrun buffer
        if (pos != length) {
            throw AudioPacketException("buffer mismath");
        }
    }

    void AudioPacket::parse_ping(const uint8_t* buffer, size_t length, size_t pos) {
        //timestamp
        VarInt varint_sessionid(&buffer[pos]);
        _ping_timestamp = varint_sessionid.Value();
    }
}
