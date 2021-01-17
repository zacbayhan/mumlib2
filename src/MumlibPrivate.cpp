//mumlib
#include "mumlib/Exceptions.hpp"
#include "mumlib_private/MumlibPrivate.h"

namespace mumlib {
	MumlibPrivate::MumlibPrivate(Callback& callback) : _callback(callback)
	{
		audioDecoderCreate(MUMBLE_AUDIO_SAMPLERATE);
        audioEncoderCreate(MUMBLE_AUDIO_SAMPLERATE, MUMBLE_OPUS_BITRATE);
	}

    //
    // ACL
    //
    bool MumlibPrivate::AclSetTokens(const std::vector<std::string>& tokens)
    {
        if (TransportGetState() != ConnectionState::CONNECTED) {
            return false;
        }

        return transportSendAuthentication(tokens);
    }

	//
	// Audio
	//

    void MumlibPrivate::AudioSend(const int16_t* pcmData, int pcmLength)
    {
        AudioSendTarget(pcmData, pcmLength, 0);
    }

    void MumlibPrivate::AudioSendTarget(const int16_t* pcmData, int pcmLength, uint32_t target)
    {
        //check encoder availability
        if (!_audio_encoder) {
            return;
        }

        //encode
        auto packet = _audio_encoder->Encode(pcmData, pcmLength, target);

        //send
        try {
            transportSendAudio(packet.data(), packet.size());
        }
        catch (const TransportException&) {}
    }

    bool MumlibPrivate::AudioSetInputSamplerate(uint32_t samplerate)
    {
        if (!_audio_encoder) {
            return false;
        }

        return _audio_encoder->SetInputSamplerate(samplerate);
    }

    bool MumlibPrivate::AudioSetOutputSamplerate(uint32_t samplerate)
    {
        if (!_audio_decoder) {
            return false;
        }

   
        return _audio_decoder->SetOutputSamplerate(samplerate);
    }

    void MumlibPrivate::audioDecoderCreate(uint32_t output_samplerate)
    {
        _audio_decoder = std::make_unique<AudioDecoder>(output_samplerate);
    }

    void MumlibPrivate::audioEncoderCreate(uint32_t input_samplerate, uint32_t output_bitrate)
    {
        _audio_encoder = std::make_unique<AudioEncoder>(input_samplerate, output_bitrate);
    }

    //
    // Channel
    //
    uint32_t MumlibPrivate::ChannelGetCurrent() const
    {
        return _channel_current;
    }

    std::vector<MumbleChannel> MumlibPrivate::ChannelGetList() const
    {
        return _channel_list;
    }

    void MumlibPrivate::channelEmplace(MumbleChannel& channel)
    {
        _channel_list.push_back(channel);
    }

    bool MumlibPrivate::ChannelExists(uint32_t channel_id) const
    {
        for (auto& channel : _channel_list) {
            if (channel.channelId == channel_id) {
                return true;
            }
        }

        return false;
    }

    void MumlibPrivate::channelErase(uint32_t channel_id)
    {
        for (int i = 0; i < _channel_list.size(); i++) {
            if (_channel_list[i].channelId == channel_id) {
                _channel_list.erase(_channel_list.begin() + i);
                return;
            }
        }
    }

    bool MumlibPrivate::ChannelJoin(uint32_t channel_id)
    {
        if (!ChannelExists(channel_id)) {
            return false;
        }

        MumbleProto::UserState userState;
        userState.set_channel_id(channel_id);

        if (!transportSendControl(MessageType::USERSTATE, userState)) {
            return false;
        }

        return true;
    }

    int32_t MumlibPrivate::ChannelFind(const std::string& channel_name) const
    {
        for (auto& channel : _channel_list) {
            if (channel.name == channel_name) {
                return channel.channelId;
            }
        }

        return -1;
    }

    void MumlibPrivate::channelSet(uint32_t channel_id)
    {
        _channel_current = channel_id;
    }


	//
	// Processing
	//
    bool MumlibPrivate::processControlPacket(MessageType messageType, const uint8_t* buffer, int length)
    {
        switch (messageType) {
        case MessageType::VERSION:
            return processControlVersionPacket(buffer, length);
        case MessageType::UDPTUNNEL:
            _logger.warn("MumlibPrivate::processControlPacket() -> UDPTUNNEL not implemented");
            break;
        case MessageType::AUTHENTICATE:
            _logger.warn("MumlibPrivate::processControlPacket() -> AUTHENTICATE not implemented");
            break;
        case MessageType::PING:
            _logger.warn("MumlibPrivate::processControlPacket() -> PING not implemented");
            break;
        case MessageType::REJECT:
            _logger.warn("MumlibPrivate::processControlPacket() -> PING not implemented");
            break;
        case MessageType::SERVERSYNC:
            return processControlServersyncPacket(buffer, length);
        case MessageType::CHANNELREMOVE:
            return processControlChannelremovePacket(buffer, length);
        case MessageType::CHANNELSTATE:
            return processControlChannelstatePacket(buffer, length);
        case MessageType::USERREMOVE:
            return processControlUserRemovePacket(buffer, length);
        case MessageType::USERSTATE:
            return processControlUserStatePacket(buffer, length);
        case MessageType::BANLIST:
            return processControlBanlistPacket(buffer, length);
        case MessageType::TEXTMESSAGE:
            return processControlTextMessagePacket(buffer, length);
        case MessageType::PERMISSIONDENIED:
            _logger.warn("MumlibPrivate::processControlPacket() -> PERMISSIONDENIED not implemented");
            break;
        case MessageType::ACL:
            _logger.warn("MumlibPrivate::processControlPacket() -> ACL not implemented");
            break;
        case MessageType::QUERYUSERS:
            _logger.warn("MumlibPrivate::processControlPacket() -> QUERYUSERS not implemented");
            break;
        case MessageType::CRYPTSETUP:
            _logger.warn("MumlibPrivate::processControlPacket() -> CRYPTSETUP not implemented");
            break;
        case MessageType::CONTEXTACTIONMODIFY:
            _logger.warn("MumlibPrivate::processControlPacket() -> CONTEXTACTIONMODIFY not implemented");
            break;
        case MessageType::CONTEXTACTION:
            _logger.warn("MumlibPrivate::processControlPacket() -> CONTEXTACTION not implemented");
            break;
        case MessageType::USERLIST:
            _logger.warn("MumlibPrivate::processControlPacket() -> USERLIST not implemented");
            break;
        case MessageType::VOICETARGET:
            _logger.warn("MumlibPrivate::processControlPacket() -> VOICETARGET not implemented");
            break;
        case MessageType::PERMISSIONQUERY:
            return processControlPermissionQueryPacket(buffer, length);
        case MessageType::CODECVERSION:
            return processControlCodecVersionPacket(buffer, length);
        case MessageType::USERSTATS:
            _logger.warn("MumlibPrivate::processControlPacket() -> USERSTATS not implemented");
            break;
        case MessageType::REQUESTBLOB:
            _logger.warn("MumlibPrivate::processControlPacket() -> REQUESTBLOB not implemented");
            break;
        case MessageType::SERVERCONFIG:
            return processControlServerconfigPacket(buffer, length);
        case MessageType::SUGGESTCONFIG:
            _logger.warn("MumlibPrivate::processControlPacket() -> SUGGESTCONFIG not implemented");
            break;
        default:
            throw MumlibException("MumlibPrivate::processControlPacket() -> unknown message type: " + to_string(static_cast<int>(messageType)));
        }

        return false;
    }

    bool MumlibPrivate::processControlBanlistPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::BanList ban_list;
        ban_list.ParseFromArray(buffer, length);
        for (int i = 0; i < ban_list.bans_size(); i++) {
            auto ban = ban_list.bans(i);

            const uint8_t* ip_data = reinterpret_cast<const uint8_t*>(ban.address().c_str());
            uint32_t ip_data_size = ban.address().size();
            int32_t duration = ban.has_duration() ? ban.duration() : -1;

            _callback.banList(
                ip_data,
                ip_data_size,
                ban.mask(),
                ban.name(),
                ban.hash(),
                ban.reason(),
                ban.start(),
                duration);
        }

        return true;
    }

    bool MumlibPrivate::processControlChannelremovePacket(const uint8_t* buffer, int length)
    {
        MumbleProto::ChannelRemove channelRemove;
        channelRemove.ParseFromArray(buffer, length);

        if (ChannelExists(channelRemove.channel_id())) {
            channelErase(channelRemove.channel_id());
        }

        _callback.channelRemove(channelRemove.channel_id());
        return true;
    }

    bool MumlibPrivate::processControlChannelstatePacket(const uint8_t* buffer, int length)
    {
        MumbleProto::ChannelState channelState;
        channelState.ParseFromArray(buffer, length);

        int32_t channel_id = channelState.has_channel_id() ? channelState.channel_id() : -1;
        int32_t parent = channelState.has_parent() ? channelState.parent() : -1;

        bool temporary = channelState.has_temporary() && channelState.temporary(); //todo make sure it's correct to assume it's false
        int position = channelState.has_position() ? channelState.position() : 0;

        vector<uint32_t> links;
        for (int i = 0; i < channelState.links_size(); ++i) {
            links.push_back(channelState.links(i));
        }

        vector<uint32_t> links_add;
        for (int i = 0; i < channelState.links_add_size(); ++i) {
            links_add.push_back(channelState.links_add(i));
        }

        vector<uint32_t> links_remove;
        for (int i = 0; i < channelState.links_remove_size(); ++i) {
            links_remove.push_back(channelState.links_remove(i));
        }

        MumbleChannel mumbleChannel;
        mumbleChannel.channelId = channel_id;
        mumbleChannel.name = channelState.name();
        mumbleChannel.description = channelState.description();

        if (!ChannelExists(channel_id)) {
            channelEmplace(mumbleChannel);
        }

        _callback.channelState(
            channelState.name(),
            channel_id,
            parent,
            channelState.description(),
            links,
            links_add,
            links_remove,
            temporary,
            position
        );

        return true;
    }

    bool MumlibPrivate::processControlCodecVersionPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::CodecVersion codecVersion;
        codecVersion.ParseFromArray(buffer, length);

        int32_t alpha = codecVersion.alpha();
        int32_t beta = codecVersion.beta();
        uint32_t prefer_alpha = codecVersion.prefer_alpha();
        int32_t opus = codecVersion.has_opus() ? codecVersion.opus() : 0;

        _callback.codecVersion(alpha, beta, prefer_alpha, opus);

        return true;
    }

    bool MumlibPrivate::processControlPermissionQueryPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::PermissionQuery permissionQuery;
        permissionQuery.ParseFromArray(buffer, length);

        int32_t channel_id = permissionQuery.has_channel_id() ? permissionQuery.channel_id() : -1;
        uint32_t permissions = permissionQuery.has_permissions() ? permissionQuery.permissions() : 0;
        uint32_t flush = permissionQuery.has_flush() ? permissionQuery.flush() : -1;

        _callback.permissionQuery(channel_id, permissions, flush);

        return true;
    }

    bool MumlibPrivate::processControlTextMessagePacket(const uint8_t* buffer, int length)
    {
        MumbleProto::TextMessage text_message;
        text_message.ParseFromArray(buffer, length);

        int32_t actor = text_message.has_actor() ? text_message.actor() : -1;

        vector<uint32_t> sessions;
        for (int i = 0; i < text_message.session_size(); ++i) {
            sessions.push_back(text_message.session(i));
        }

        vector<uint32_t> channel_ids;
        for (int i = 0; i < text_message.channel_id_size(); ++i) {
            channel_ids.push_back(text_message.channel_id(i));
        }

        vector<uint32_t> tree_ids;
        for (int i = 0; i < text_message.tree_id_size(); ++i) {
            tree_ids.push_back(text_message.tree_id(i));
        }

        _callback.textMessage(actor, sessions, channel_ids, tree_ids, text_message.message());

        return true;
    }

    bool MumlibPrivate::processControlVersionPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::Version version;
        version.ParseFromArray(buffer, length);
        _callback.version(
            version.version() >> 16,
            version.version() >> 8 & 0xff,
            version.version() & 0xff,
            version.release(),
            version.os(),
            version.os_version());

        return true;
    }

    bool MumlibPrivate::processControlUserRemovePacket(const uint8_t* buffer, int length)
    {
        MumbleProto::UserRemove user_remove;
        user_remove.ParseFromArray(buffer, length);

        int32_t actor = user_remove.has_actor() ? user_remove.actor() : -1;
        bool ban = user_remove.has_ban() && user_remove.ban(); //todo make sure it's correct to assume it's false

        if (UserExists(user_remove.session())) {
            userErase(user_remove.session());
        }

        _callback.userRemove(
            user_remove.session(),
            actor,
            user_remove.reason(),
            ban
        );

        return true;
    }

    bool MumlibPrivate::processControlUserStatePacket(const uint8_t* buffer, int length)
    {
        MumbleProto::UserState userState;
        userState.ParseFromArray(buffer, length);

        // There are far too many things in this structure. Culling to the ones that are probably important
        int32_t session = userState.has_session() ? userState.session() : -1;
        int32_t actor = userState.has_actor() ? userState.actor() : -1;
        int32_t user_id = userState.has_user_id() ? userState.user_id() : -1;
        int32_t channel_id = userState.has_channel_id() ? userState.channel_id() : -1;
        int32_t mute = userState.has_mute() ? userState.mute() : -1;
        int32_t deaf = userState.has_deaf() ? userState.deaf() : -1;
        int32_t suppress = userState.has_suppress() ? userState.suppress() : -1;
        int32_t self_mute = userState.has_self_mute() ? userState.self_mute() : -1;
        int32_t self_deaf = userState.has_self_deaf() ? userState.self_deaf() : -1;
        int32_t priority_speaker = userState.has_priority_speaker() ? userState.priority_speaker() : -1;
        int32_t recording = userState.has_recording() ? userState.recording() : -1;

        if (session == sessionGet()) {
            channelSet(channel_id);
        }

        MumbleUser mumbleUser;
        mumbleUser.name = userState.name();
        mumbleUser.sessionId = session;

        if (!UserExists(session)) {
            userEmplace(mumbleUser);
        }

        _callback.userState(session,
            actor,
            userState.name(),
            user_id,
            channel_id,
            mute,
            deaf,
            suppress,
            self_mute,
            self_deaf,
            userState.comment(),
            priority_speaker,
            recording);

        return true;
    }

    bool MumlibPrivate::processControlServerconfigPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::ServerConfig serverConfig;
        serverConfig.ParseFromArray(buffer, length);

        _server_maxbandwidth  = serverConfig.has_max_bandwidth() ? serverConfig.max_bandwidth() : 0;
        _server_allowhtml     = serverConfig.has_allow_html() ? serverConfig.allow_html() : 0;
        _server_welcometext   = serverConfig.welcome_text();
        _server_imagemessagelength = serverConfig.has_image_message_length() ? serverConfig.image_message_length() : 0;
        _server_messagelength = serverConfig.has_message_length() ? serverConfig.message_length() : 0;

        _callback.serverConfig(
            _server_maxbandwidth, 
            _server_welcometext, 
            _server_allowhtml, 
            _server_messagelength,
            _server_imagemessagelength);

        return true;
    }

    bool MumlibPrivate::processControlServersyncPacket(const uint8_t* buffer, int length)
    {
        MumbleProto::ServerSync serverSync;
        serverSync.ParseFromArray(buffer, length);

        _session_id = serverSync.session();

        _callback.serverSync(
            serverSync.welcome_text(),
            serverSync.session(),
            serverSync.max_bandwidth(),
            serverSync.permissions()
        );

        return true;
    }

	bool MumlibPrivate::processAudioPacket(AudioPacket& packet)
	{
        if (packet.GetHeaderType() == AudioPacketType::Opus) {
            auto  [buf, len] = _audio_decoder->Process(packet.GetAudioPayload());
            if (len < 0) {
                _logger.warn("MumlibPrivate::processAudioPacket() -> failed to decode Opus");
                return false;
            }

            _callback.audio(
                packet.GetHeaderTarget(),
                packet.GetAudioSessionId(),
                packet.GetAudioSequenceNumber(),
                buf,
                len);
        }
        else if (packet.GetHeaderType() == AudioPacketType::Ping) {
            //TODO: callback for ping
        }
        else {
            _logger.warn("MumlibPrivate::processAudioPacket() -> codec not implemented");
            _callback.unsupportedAudio(
                packet.GetHeaderTarget(),
                packet.GetAudioSessionId(),
                packet.GetAudioSequenceNumber(),
                packet.GetAudioPayload().data(),
                packet.GetAudioPayload().size()
            );
        }

        return true;
	}

    //
    // User
    //

    std::optional<MumbleUser> MumlibPrivate::UserGet(int32_t session_id) const
    {
        for (const auto& user : _user_list) {
            if (user.sessionId == session_id) {
                return { user };
            }
        }

        return {};
    }

    std::vector<MumbleUser> MumlibPrivate::UserGetList() const
    {
        return _user_list;
    }

    bool MumlibPrivate::UserExists(uint32_t user_id) const
    {
        for (auto& user : _user_list) {
            if (user.sessionId == user_id) {
                return true;
            }
        }

        return false;
    }

    void MumlibPrivate::userEmplace(MumbleUser& user)
    {
        _user_list.push_back(user);
    }

    void MumlibPrivate::userErase(uint32_t user_id)
    {
        for (size_t i = 0; i < _user_list.size(); i++) {
            if (_user_list[i].sessionId == user_id) {
                _user_list.erase(_user_list.begin() + i);
                return;
            }
        }
    }

    int32_t MumlibPrivate::UserFind(const std::string& user_name) const
    {
        for (auto& user : _user_list) {
            if (user.name == user_name) {
                return user.sessionId;
            }
        }

        return -1;
    }

    bool MumlibPrivate::UserSendState(UserState field, bool val)
    {
        MumbleProto::UserState userState;

        switch (field) {
        case UserState::MUTE:
            userState.set_mute(val);
            break;
        case UserState::DEAF:
            userState.set_deaf(val);
            break;
        case UserState::SUPPRESS:
            userState.set_suppress(val);
            break;
        case UserState::SELF_MUTE:
            userState.set_self_mute(val);
            break;
        case UserState::SELF_DEAF:
            userState.set_self_deaf(val);
            break;
        case UserState::PRIORITY_SPEAKER:
            userState.set_priority_speaker(val);
            break;
        case UserState::RECORDING:
            userState.set_recording(val);
            break;
        default:
            // in any other case, just ignore the command
            return false;
        }

        if (!transportSendControl(MessageType::USERSTATE, userState)) {
            return false;
        }

        return true;
    }

    bool MumlibPrivate::UserSendState(UserState field, const std::string& val)
    {
        MumbleProto::UserState userState;

        // if comment longer than 128 bytes, we need to set the SHA1 hash
        // http://www.askyb.com/cpp/openssl-sha1-hashing-example-in-cpp/
        unsigned char digest[SHA_DIGEST_LENGTH]{};
        char mdString[SHA_DIGEST_LENGTH * 2 + 1];
        SHA1((unsigned char*)val.c_str(), val.size(), digest);
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
        }

        switch (field) {
        case UserState::COMMENT:
            if (val.size() < 128) {
                userState.set_comment(val);
            }
            else {
                userState.set_comment_hash(mdString);
            }
            break;
        default:
            // in any other case, just ignore the command
            return false;
        }

        if (!transportSendControl(MessageType::USERSTATE, userState)) {
            return false;
        }

        return true;
    }

    //
    // Session
    //

    uint32_t MumlibPrivate::sessionGet() const
    {
        return _session_id;
    }


    //
    // Text
    //
    bool MumlibPrivate::TextSend(const std::string& message)
    {
        MumbleProto::TextMessage textMessage;
        textMessage.set_actor(sessionGet());
        textMessage.add_channel_id(ChannelGetCurrent());
        textMessage.set_message(message);

        if (!transportSendControl(MessageType::TEXTMESSAGE, textMessage)) {
            return false;
        }
        
        return true;
    }


	//
	// Transport
	//
	bool MumlibPrivate::TransportConnect(const std::string& host, uint16_t port, const std::string& user, const std::string& password)
	{
        if (TransportGetState() == ConnectionState::CONNECTED ||
            TransportGetState() == ConnectionState::IN_PROGRESS ||
            TransportGetState() == ConnectionState::DISCONNECTING) {
            return false;
        }

        _channel_current = 0;
        _session_id = 0;

		if (!_transport) {
			transportCreate();
		}
		_transport->connect(host, port, user, password);
	}

	void MumlibPrivate::TransportDisconnect()
	{
		if (_transport) {
			_transport->disconnect();
		}
		_transport.reset();

        _channel_current = 0;
        _session_id = 0;
	}

	ConnectionState MumlibPrivate::TransportGetState() const
	{
		if (!_transport) {
			return ConnectionState::NOT_CONNECTED;
		}

		return _transport->getConnectionState();
	}

	void MumlibPrivate::TransportRun()
	{
		_transport->run();
	}

	void MumlibPrivate::TransportSetCert(const std::string& cert)
	{
		_transport_cert = cert;
	}

	void MumlibPrivate::TransportSetKey(const std::string& key)
	{
		_transport_key = key;
	}

	void MumlibPrivate::transportCreate()
	{
		_transport = std::make_unique<Transport>(
			boost::bind(&MumlibPrivate::processControlPacket, this, boost::placeholders::_1, boost::placeholders::_2, boost::placeholders::_3),
			boost::bind(&MumlibPrivate::processAudioPacket, this, boost::placeholders::_1),
			_transport_cert,
			_transport_key);
	}

    bool MumlibPrivate::transportSendAuthentication(const std::vector<std::string>& tokens)
    {
        if (!_transport) {
            return false;
        }

        _transport->sendAuthentication({ tokens });
        return true;
    }

    bool MumlibPrivate::transportSendControl(MessageType type, google::protobuf::Message& message)
    {
        if (!_transport) {
            return false;
        }

        _transport->sendControlMessage(type, message);
        return true;
    }

    bool MumlibPrivate::transportSendAudio(const uint8_t* data, size_t len)
    {
        if (!_transport) {
            return false;
        }

        _transport->sendEncodedAudioPacket(data, len);
        return true;
    }

    //
    // Voicetarget
    //

    bool MumlibPrivate::VoicetargetSet(int targetId, VoiceTargetType type, int id)
    {
        MumbleProto::VoiceTarget_Target voiceTargetTarget;
        
        switch (type) {
            case VoiceTargetType::CHANNEL: 
                voiceTargetTarget.set_channel_id(id);
                voiceTargetTarget.set_children(true);
                break;
            case VoiceTargetType::USER:
                voiceTargetTarget.add_session(id);
                break;
            default:
                return false;
        }
        _voiceTarget.set_id(targetId);
        _voiceTarget.add_targets()->CopyFrom(voiceTargetTarget);
        
        if (!transportSendControl(MessageType::VOICETARGET, _voiceTarget)) {
            return false;
        }
        return true;
    }

    bool MumlibPrivate::VoicetargetSet(int targetId, VoiceTargetType type, const std::string& name)
    {
        int id = -1;
        switch (type) {
        case VoiceTargetType::CHANNEL:
            id = ChannelFind(name);
            break;
        case VoiceTargetType::USER:
            id = UserFind(name);
            break;
        default:
            break;
        }

        if (id < 0) {
            return false;
        }

        return VoicetargetSet(targetId, type, id);
    }

}
