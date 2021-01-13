#pragma once

//stdlib
#include <chrono>
#include <map>

//opus
#include <opus/opus.h>

//mumlib
#include "mumlib.hpp"
#include "mumlib/Constants.hpp"
#include "mumlib/Logger.hpp"

namespace mumlib {

    struct IncomingAudioPacket {
        AudioPacketType type;
        int target;
        int64_t sessionId;
        int64_t sequenceNumber;
        uint8_t *audioPayload;
        int audioPayloadLength;
    };

    class Audio {
    public:
        //mark as non-copyable
        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        explicit Audio(int sampleRate=DEFAULT_OPUS_SAMPLE_RATE,
                       int bitrate=DEFAULT_OPUS_ENCODER_BITRATE,
                       int channels=DEFAULT_OPUS_NUM_CHANNELS);

        virtual ~Audio();

        IncomingAudioPacket decodeIncomingAudioPacket(uint8_t *inputBuffer, int inputBufferLength);

        std::pair<int, bool> decodeOpusPayload(uint8_t *inputBuffer,
                                               int inputLength,
                                               int16_t *pcmBuffer,
                                               int pcmBufferSize);

        int encodeAudioPacket(
                int target,
                int16_t *inputPcmBuffer,
                int inputLength,
                uint8_t *outputBuffer,
                int outputBufferSize = MAX_UDP_LENGTH);

        void setOpusEncoderBitrate(int bitrate);

        int getOpusEncoderBitrate();

        void resetEncoder();

    private:
        uint32_t _samplerate = 0;

        Logger logger = Logger("Mumlib.audio");

        OpusDecoder * opusDecoder = nullptr;
        OpusEncoder * opusEncoder = nullptr;

        int64_t outgoingSequenceNumber = 0;

        std::chrono::time_point<std::chrono::system_clock> lastEncodedAudioPacketTimestamp;
    };
}
