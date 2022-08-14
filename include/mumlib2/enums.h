// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <cstdint>

namespace mumlib2 {
    enum class MessageType {
        VERSION = 0,
        UDPTUNNEL = 1,
        AUTHENTICATE = 2,
        PING = 3,
        REJECT = 4,
        SERVERSYNC = 5,
        CHANNELREMOVE = 6,
        CHANNELSTATE = 7,
        USERREMOVE = 8,
        USERSTATE = 9,
        BANLIST = 10,
        TEXTMESSAGE = 11,
        PERMISSIONDENIED = 12,
        ACL = 13,
        QUERYUSERS = 14,
        CRYPTSETUP = 15,
        CONTEXTACTIONMODIFY = 16,
        CONTEXTACTION = 17,
        USERLIST = 18,
        VOICETARGET = 19,
        PERMISSIONQUERY = 20,
        CODECVERSION = 21,
        USERSTATS = 22,
        REQUESTBLOB = 23,
        SERVERCONFIG = 24,
        SUGGESTCONFIG = 25
    };

    enum class ConnectionState {
        NOT_CONNECTED = 0, 
        IN_PROGRESS = 1,
        CONNECTED = 2,
        DISCONNECTING = 3,
        FAILED = 4
    };

	enum class AudioPacketType : uint8_t {
		CeltAplha = 0b00000000,
		Ping      = 0b00100000,
		Speex     = 0b01000000,
        CeltBeta  = 0b01100000,
        Opus      = 0b10000000,
	};

    enum class UserState {
        MUTE,
        DEAF,
        SUPPRESS,
        SELF_MUTE,
        SELF_DEAF,
        COMMENT,
        PRIORITY_SPEAKER,
        RECORDING
    };

    enum class VoiceTargetType {
        CHANNEL,
        USER
    };

    enum class PingState {
        PING,
        PONG,
        NONE
    };

}