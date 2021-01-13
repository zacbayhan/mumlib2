//mumlib
#include "mumlib.hpp"
#include "mumlib_private/MumlibPrivate.h"

namespace mumlib {

    Mumlib::Mumlib(Callback &callback) {
        MumlibConfiguration conf;
        impl = new MumlibPrivate(callback, conf);
    }

    Mumlib::Mumlib(Callback &callback, MumlibConfiguration &configuration){
        impl = new MumlibPrivate(callback, configuration);
    }

    Mumlib::~Mumlib() {
        disconnect();
        delete impl;
    }

    ConnectionState Mumlib::getConnectionState() {
        return impl->GetConnectionState();
    }

    int Mumlib::getChannelId() {
        return impl->GetChannelId();
    }

    vector<mumlib::MumbleUser> Mumlib::getListAllUser() {
        return impl->GetUsers();
    }

    vector<mumlib::MumbleChannel> Mumlib::getListAllChannel() {
        return impl->GetChannels();
    }

    void Mumlib::connect(string host, int port, string user, string password) {
        impl->Connect(host, port, user, password);
    }

    void Mumlib::disconnect() {
        impl->Disconnect();
    }

    void Mumlib::run() {
        impl->Run();
    }

    void Mumlib::sendAudioData(int16_t *pcmData, int pcmLength) {
        impl->SendAudioData(pcmData, pcmLength);
    }

    void Mumlib::sendAudioDataTarget(int targetId, int16_t *pcmData, int pcmLength) {
        impl->SendAudioDataTarget(targetId, pcmData,pcmLength);
    }

    void Mumlib::sendTextMessage(string message) {
        impl->SendTextMessage(message);
    }

    void Mumlib::joinChannel(int channelId) {
        impl->JoinChannel(channelId);
    }

    void Mumlib::joinChannel(string name) {
        Mumlib::joinChannel(Mumlib::getChannelIdBy(name));
    }

    void Mumlib::sendVoiceTarget(int targetId, VoiceTargetType type, int id) {
        impl->SendVoiceTarget(targetId, type, id);
    }

    void Mumlib::sendVoiceTarget(int targetId, VoiceTargetType type, string name, int &error) {
        impl->SendVoiceTarget(targetId, type, name, error);
    }

    void Mumlib::sendUserState(mumlib::UserState field, bool val) {
        impl->SendUserState(field,val);
    }

    void Mumlib::sendUserState(mumlib::UserState field, std::string val) {
        impl->SendUserState(field, val);
    }

    int Mumlib::getChannelIdBy(string name) {
        return impl->GetChannelIdBy(name);
    }

    int Mumlib::getUserIdBy(string name) {
        return impl->GetUserIdBy(name);
    }

    bool Mumlib::isSessionIdValid(int sessionId) {
        return impl->IsSessionIdValid(sessionId);
    }

    bool Mumlib::isChannelIdValid(int channelId) {
        return impl->IsChannelIdValid(channelId);
    }
}
