// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <array>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

//boost
#include <asio.hpp>
#include <asio/ssl.hpp>

//protobuf
#include <google/protobuf/message.h>

//mumlib
#include "mumlib2/constants.h"
#include "mumlib2/enums.h"
#include "mumlib2/logger.h"
#include "mumlib2_private/audio_packet.h"
#include "mumlib2_private/crypto_state.h"
#include "mumlib2_private/transport_ssl_context.h"
#include "mumlib2_private/varint.h"


namespace mumlib2 {

    class Transport {
    public:
        Transport(
                  std::function<bool(MessageType, uint8_t*, int)> processControlMessageFunc,
                  std::function<bool(AudioPacket&)>                processEncodedAudioPacketFunction,
                  std::string cert_file = "",
                  std::string privkey_file = "");

        ~Transport();

        Transport(const Transport&) = delete;
        Transport& operator=(const Transport&) = delete;

        void connect(const std::string& host, int port, const std::string& user, const std::string& password);

        void disconnect();

        ConnectionState getConnectionState() {
            return state;
        }

        bool isUdpActive();

        void sendControlMessage(MessageType type, google::protobuf::Message &message);

        void sendEncodedAudioPacket(const uint8_t *buffer, int length);

        void run(){
            ioService.run();
        }

        void sendAuthentication(std::optional<const std::vector<std::string>> tokens);

    private:
        Logger logger;

        asio::io_service ioService;

        std::pair<std::string, int> connectionParams;

        std::pair<std::string, std::string> credentials;

        std::function<bool(MessageType, uint8_t*, int)> processMessageFunction;

        std::function<bool(AudioPacket&)> processEncodedAudioPacketFunction;

        volatile bool udpActive;

        ConnectionState state = ConnectionState::NOT_CONNECTED;
        PingState ping_state = PingState::NONE;

        asio::ip::udp::socket udpSocket;
        asio::ip::udp::endpoint udpReceiverEndpoint;
        uint8_t udpIncomingBuffer[MUMBLE_UDP_MAXLENGTH];
        CryptState cryptState;

        asio::ssl::context sslContext;
        SslContextHelper sslContextHelper;
        asio::ssl::stream<asio::ip::tcp::socket> sslSocket;
        std::array<uint8_t, MUMBLE_TCP_MAXLENGTH> sslIncomingBuffer;


        asio::steady_timer pingTimer;
        std::chrono::time_point<std::chrono::system_clock> lastReceivedUdpPacketTimestamp;

        void pingTimerTick(const std::error_code &e);

        void sslConnectHandler(const std::error_code &error);

        void sslHandshakeHandler(const std::error_code &error);

        void doReceiveSsl();

        void sendSsl(uint8_t *buff, int length);

        void sendSslAsync(uint8_t *buff, int length);

        void sendControlMessagePrivate(MessageType type, google::protobuf::Message &message);

        void sendSslPing();

        void sendVersion();

        void processMessageInternal(MessageType messageType, uint8_t *buffer, int length);

        void doReceiveUdp();

        void sendUdpAsync(const uint8_t *buff, int length);

        void sendUdpPing();

        void throwTransportException(std::string message);
    };


}
