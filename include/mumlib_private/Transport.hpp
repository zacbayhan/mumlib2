#pragma once

//stdlib
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

//boost
#include <boost/noncopyable.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/pool/pool.hpp>

//protobuf
#include <google/protobuf/message.h>

//mumlib
#include "mumlib/Constants.hpp"
#include "mumlib/Enums.hpp"
#include "mumlib/Logger.hpp"
#include "mumlib_private/AudioPacket.hpp"
#include "mumlib_private/CryptState.hpp"
#include "mumlib_private/SslContextHelper.h"
#include "mumlib_private/VarInt.hpp"


namespace mumlib {
    using namespace boost::asio;
    using namespace boost::asio::ip;

    class Transport : boost::noncopyable {
    public:
        Transport(
                  std::function<bool(MessageType, uint8_t*, int)> processControlMessageFunc,
                  std::function<bool(AudioPacket&)>                processEncodedAudioPacketFunction,
                  std::string cert_file = "",
                  std::string privkey_file = "");

        ~Transport();

        void connect(const std::string& host, int port, const std::string& user, const std::string& password, const std::vector<std::string>& tokens);

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

        boost::asio::io_service ioService;

        std::pair<std::string, int> connectionParams;

        std::pair<std::string, std::string> credentials;

        std::vector<std::string> _tokens;

        std::function<bool(MessageType, uint8_t*, int)> processMessageFunction;

        std::function<bool(AudioPacket&)> processEncodedAudioPacketFunction;

        volatile bool udpActive;

        ConnectionState state = ConnectionState::NOT_CONNECTED;
        PingState ping_state = PingState::NONE;

        udp::socket udpSocket;
        ip::udp::endpoint udpReceiverEndpoint;
        uint8_t udpIncomingBuffer[MUMBLE_UDP_MAXLENGTH];
        CryptState cryptState;

        ssl::context sslContext;
        SslContextHelper sslContextHelper;
        ssl::stream<tcp::socket> sslSocket;
        uint8_t *sslIncomingBuffer;

        deadline_timer pingTimer;
        std::chrono::time_point<std::chrono::system_clock> lastReceivedUdpPacketTimestamp;

        boost::pool<> asyncBufferPool;

        void pingTimerTick(const boost::system::error_code &e);

        void sslConnectHandler(const boost::system::error_code &error);

        void sslHandshakeHandler(const boost::system::error_code &error);

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
