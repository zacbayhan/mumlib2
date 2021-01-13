//stdlib
#include <chrono>
#include <thread>

//mumlib
#include <mumlib.hpp>
#include <mumlib/Exceptions.hpp>
#include <mumlib/Logger.hpp>

class MyCallback : public mumlib::BasicCallback {
public:
    mumlib::Mumlib *mum;

    mumlib::Logger logger = mumlib::Logger("");

    virtual void audio(int target,
                       int sessionId,
                       int sequenceNumber,
                       int16_t *pcm_data,
                       uint32_t pcm_data_size) override {
        logger.notice("Received audio: pcm_data_size: %d", pcm_data_size);
        mum->sendAudioData(pcm_data, pcm_data_size);
    }

    virtual void textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
            std::string message) override {
        mumlib::BasicCallback::textMessage(actor, session, channel_id, tree_id, message);
        logger.notice("Received text message: %s", message.c_str());
        mum->sendTextMessage("someone said: " + message);
    }
};

int main(int argc, char *argv[]) {
    mumlib::Logger logger = mumlib::Logger("");

    if (argc < 5) {
        logger.crit("Usage: %s {server} {port} {username} {password} [{certfile} {keyfile}]", argv[0]);
        return 1;
    }
  
    mumlib::MumlibConfiguration conf;
    conf.opusEncoderBitrate = 16000;

    std::string server = argv[1];
    uint16_t port = std::stoi(argv[2]);
    std::string username = argv[3];
    std::string password = argv[4];


    if (argc >= 7) {
        conf.cert_file = argv[5];
        conf.privkey_file = argv[6];
    }

    MyCallback myCallback;
    while (true) {
        try {
            mumlib::Mumlib mum(myCallback, conf);
            myCallback.mum = &mum;
            mum.connect(server, port, username, password);
            mum.run();

        } catch (mumlib::TransportException &exp) {
            logger.error("TransportException: %s.", exp.what());
            logger.notice("Attempting to reconnect in 5 s.");
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}
