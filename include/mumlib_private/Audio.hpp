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
        
        //ctor/dtor
        explicit Audio(uint32_t encoding_bitrate);
        ~Audio();

        int decoderProcess(const std::vector<uint8_t>& input, int16_t* pcmBuffer, int pcmBufferSize);

        int encodeAudioPacket(
            int target,
            int16_t* inputPcmBuffer,
            int inputLength,
            uint8_t* outputBuffer,
            int outputBufferSize = MAX_UDP_LENGTH);

    private:
        //encoder
        void encoderCreate(uint32_t sampleRate, uint32_t channels);
        void encoderDestroy();
        void encoderReset();
        void encoderSetBitrate(uint32_t bitrate);
       
        //decoder
        void decoderCreate(uint32_t sampleRate, uint32_t channels);
        void decoderDestroy();
    private:
        Logger logger = Logger("Mumlib.audio");

        OpusDecoder* _decoder = nullptr;
        OpusEncoder* _encoder = nullptr;

        int64_t outgoingSequenceNumber = 0;
        std::chrono::time_point<std::chrono::system_clock> lastEncodedAudioPacketTimestamp;
    };
}
