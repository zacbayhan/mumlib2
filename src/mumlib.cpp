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

    std::optional<MumbleUser> Mumlib::UserGet(int32_t session_id)
    {
        return impl->UserGet(session_id);
    }

    ConnectionState Mumlib::getConnectionState() {
        return impl->TransportGetState();
    }

    int Mumlib::getChannelId() {
        return impl->ChannelGetCurrent();
    }

    vector<mumlib::MumbleUser> Mumlib::getListAllUser() {
        return impl->UserGetList();
    }

    vector<mumlib::MumbleChannel> Mumlib::getListAllChannel() {
        return impl->ChannelGetList();
    }

    void Mumlib::connect(string host, int port, string user, string password) {
        impl->TransportConnect(host, port, user, password);
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

    void Mumlib::joinChannel(int channelId) {
        impl->ChannelJoin(channelId);
    }

    void Mumlib::joinChannel(string name) {
        Mumlib::joinChannel(Mumlib::getChannelIdBy(name));
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
