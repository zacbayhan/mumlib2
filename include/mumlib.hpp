#pragma once

//stdlib
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

//mumlib
#include "mumlib/Callback.hpp"
#include "mumlib/Constants.hpp"
#include "mumlib/Enums.hpp"
#include "mumlib/Exceptions.hpp"
#include "mumlib/Logger.hpp"
#include "mumlib/Structs.hpp"

namespace mumlib {

    class MumlibPrivate;

    class Mumlib {
    public:
        //mark as non-copyable
        Mumlib(const Mumlib&) = delete;
        Mumlib& operator=(const Mumlib&) = delete;

        explicit Mumlib(Callback &callback);

        virtual ~Mumlib();

        //acl
        bool AclSetTokens(const std::vector<std::string>& tokens);

        //audio
        bool AudioSetInputSamplerate(uint32_t samplerate);
        bool AudioSetOutputSamplerate(uint32_t samplerate);

        //channel
        std::string ChannelCurrentGetName();
        int32_t ChannelCurrentGetId();
        bool ChannelJoin(const std::string& channel_name);
        bool ChannelJoin(int channel_id);

        //user
        std::optional<MumbleUser> UserGet(int32_t session_id);
        std::vector<MumbleUser> UserGetInChannel(int32_t channel_id);
        std::vector<MumbleUser> UserGetInChannel(const std::string& channel_name);
        [[nodiscard]] bool UserMuted(const std::string& user_name);
        [[nodiscard]] bool UserMuted(int32_t user_id);
        bool UserMute(const std::string& user_name, bool mute_state);
        bool UserMute(int32_t user_id, bool mute_state);

        //
        bool connect(string host, int port, string user, string password);

        void disconnect();

        void run();

        ConnectionState getConnectionState();

        vector<MumbleUser> getListAllUser();

        vector<MumbleChannel> getListAllChannel();

        void sendAudioData(const int16_t *pcmData, int pcmLength);

        void sendAudioDataTarget(int targetId, const int16_t *pcmData, int pcmLength);

        void sendTextMessage(std::string message);

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
