#pragma once

//stdlib
#include <cstdint>
#include <stdexcept>
#include <string>

//mumlib
#include "mumlib/Callback.hpp"
#include "mumlib/Constants.hpp"
#include "mumlib/Enums.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib/Logger.hpp"

namespace mumlib {

    struct MumlibConfiguration {
        int opusEncoderBitrate = DEFAULT_OPUS_ENCODER_BITRATE;
        int opusSampleRate = DEFAULT_OPUS_SAMPLE_RATE;
        int opusChannels = DEFAULT_OPUS_NUM_CHANNELS;
        std::string cert_file = "";
        std::string privkey_file = "";
        // additional fields will be added in the future
    };

    struct MumbleUser {
        int32_t sessionId;
        std::string name;
    };

    struct MumbleChannel {
        int32_t channelId;
        std::string name;
        std::string description;
    };

    class MumlibPrivate;

    class Mumlib {
    public:
        //mark as non-copyable
        Mumlib(const Mumlib&) = delete;
        Mumlib& operator=(const Mumlib&) = delete;

        explicit Mumlib(Callback &callback);

        explicit Mumlib(Callback &callback, MumlibConfiguration &configuration);

        virtual ~Mumlib();

        void connect(string host, int port, string user, string password);

        void disconnect();

        void run();

        ConnectionState getConnectionState();

        int getChannelId();

        vector<MumbleUser> getListAllUser();

        vector<MumbleChannel> getListAllChannel();

        void sendAudioData(int16_t *pcmData, int pcmLength);

        void sendAudioDataTarget(int targetId, int16_t *pcmData, int pcmLength);

        void sendTextMessage(std::string message);

        void joinChannel(int channelId);

        void joinChannel(std::string channelName);

        void sendVoiceTarget(int targetId, mumlib::VoiceTargetType type, int sessionId);

        void sendVoiceTarget(int targetId, mumlib::VoiceTargetType type, std::string name, int &error);

        void sendUserState(mumlib::UserState state, bool val);

        void sendUserState(mumlib::UserState state, std::string value);

        bool isSessionIdValid(int sessionId);
    private:
        MumlibPrivate *impl;

        int getChannelIdBy(std::string channelName);

        int getUserIdBy(std::string userName);

        bool isChannelIdValid(int channelId);
    };
}
