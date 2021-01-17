//mumlib
#include "mumlib.hpp"
#include "mumlib_private/MumlibPrivate.h"

namespace mumlib {

    Mumlib::Mumlib(Callback& callback) {
        impl = new MumlibPrivate(callback);
    }

    Mumlib::~Mumlib() {
        disconnect();
        delete impl;
    }

    //
    // ACL
    //
    bool Mumlib::AclSetTokens(const std::vector<std::string>& tokens)
    {
        return impl->AclSetTokens(tokens);
    }

    //
    // Audio
    //
    bool Mumlib::AudioSetInputSamplerate(uint32_t samplerate)
    {
        return impl->AudioSetInputSamplerate(samplerate);
    }

    bool Mumlib::AudioSetOutputSamplerate(uint32_t samplerate)
    {
        return impl->AudioSetOutputSamplerate(samplerate);
    }
    
    //
    // Channel
    //
    bool Mumlib::ChannelJoin(int channelId) {
        return impl->ChannelJoin(channelId);
    }

    std::string Mumlib::ChannelCurrentGetName()
    {
        auto current_id = ChannelCurrentGetId();
        for (const auto& chan : impl->ChannelGetList()) {
            if (chan.channelId == current_id) {
                return chan.name;
            }
        }

        return "";
    }

    int32_t Mumlib::ChannelCurrentGetId()
    {
        return impl->ChannelGetCurrent();
    }

    bool Mumlib::ChannelJoin(const std::string& channel_name) {
        auto id = getChannelIdBy(channel_name);
        if (id < 0) {
            return false;
        }

        return ChannelJoin(id);
    }

    //
    // User
    //
    std::optional<MumbleUser> Mumlib::UserGet(int32_t session_id)
    {
        return impl->UserGet(session_id);
    }

    ConnectionState Mumlib::getConnectionState() {
        return impl->TransportGetState();
    }

    vector<mumlib::MumbleUser> Mumlib::getListAllUser() {
        return impl->UserGetList();
    }

    vector<mumlib::MumbleChannel> Mumlib::getListAllChannel() {
        return impl->ChannelGetList();
    }

    bool Mumlib::connect(string host, int port, string user, string password) {
        return impl->TransportConnect(host, port, user, password);
    }

    void Mumlib::disconnect() {
        impl->TransportDisconnect();
    }

    void Mumlib::run() {
        impl->TransportRun();
    }

    void Mumlib::sendAudioData(const int16_t *pcmData, int pcmLength) {
        impl->AudioSend(pcmData, pcmLength);
    }

    void Mumlib::sendAudioDataTarget(int targetId, const int16_t *pcmData, int pcmLength) {
        impl->AudioSendTarget(pcmData, pcmLength, targetId);
    }

    void Mumlib::sendTextMessage(string message) {
        impl->TextSend(message);
    }


    void Mumlib::sendVoiceTarget(int targetId, VoiceTargetType type, int id) {
        impl->VoicetargetSet(targetId, type, id);
    }

    void Mumlib::sendVoiceTarget(int targetId, VoiceTargetType type, string name, int &error) {
        error = impl->VoicetargetSet(targetId, type, name);
    }

    void Mumlib::sendUserState(mumlib::UserState field, bool val) {
        impl->UserSendState(field,val);
    }

    void Mumlib::sendUserState(mumlib::UserState field, std::string val) {
        impl->UserSendState(field, val);
    }

    int Mumlib::getChannelIdBy(string name) {
        return impl->ChannelFind(name);
    }

    int Mumlib::getUserIdBy(string name) {
        return impl->UserFind(name);
    }

    bool Mumlib::isSessionIdValid(int sessionId) {
        return impl->UserExists(sessionId);
    }

    bool Mumlib::isChannelIdValid(int channelId) {
        return impl->ChannelExists(channelId);
    }
}
