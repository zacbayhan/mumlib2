#include "mumlib_private/AudioPacket.hpp"
#include "mumlib_private/VarInt.hpp"

namespace mumlib {
    //
    // Ctor
    //
	AudioPacket::AudioPacket(const uint8_t* buffer, size_t length)
	{
        _type = static_cast<AudioPacketType>(buffer[0] & packet_type_mask);
        _target = buffer[0] & packet_target_mask;

        switch (_type) {
            case AudioPacketType::CeltAplha:
            case AudioPacketType::Speex:
            case AudioPacketType::CeltBeta:
            case AudioPacketType::Opus:
                parse_audio_in(buffer, length, 1);
            case AudioPacketType::Ping:
            default:
                return;
        }
	}

    //
    // Getters
    //    
    const std::vector<uint8_t>& AudioPacket::GetAudioPayload()
    {
        return _payload;
    }

    int64_t AudioPacket::GetAudioSessionId()
    {
        return _session;
    }

    int64_t AudioPacket::GetAudioSequenceNumber()
    {
        return _sequence;
    }

    bool AudioPacket::GetAudioLastFlag()
    {
        return _audio_last;
    }

    uint8_t AudioPacket::GetTarget()
    {
        return _target;
    }

    AudioPacketType AudioPacket::GetType()
    {
        return _type;
    }

    //
    // Parser
    //
    void AudioPacket::parse_audio_in(const uint8_t* buffer, size_t length, size_t pos)
    {
        //session ID
        VarInt varint_sessionid(&buffer[pos]);
        _session = varint_sessionid.getValue();
        pos += varint_sessionid.getEncoded().size();

        //sequence number
        VarInt varint_sequencenumber(&buffer[pos]);
        _sequence = varint_sequencenumber.getValue();
        pos += varint_sequencenumber.getEncoded().size();

        //opus
        if (GetType() == AudioPacketType::Opus) {
            VarInt varint_sequencenumber(&buffer[pos]);
            int64_t header = varint_sequencenumber.getValue();
            pos += varint_sequencenumber.getEncoded().size();

            auto opus_length = header & _audio_opus_length_mask;
            auto _audio_last = (header & _audio_opus_last_mask) == _audio_opus_last_mask;
            _payload = std::vector<uint8_t>(buffer + pos, buffer + pos + opus_length);
            pos += opus_length;
        }

        //position
        if (pos < length) {
            for (size_t i = 0; i < _position.size(); i++) {
                _position[i] = reinterpret_cast<const float*>(buffer + pos)[i];
            }
        }
    }
}
