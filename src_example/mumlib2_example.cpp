//stdlib
#include <chrono>
#include <thread>

//mumlib
#include <mumlib2.h>

class MyCallback : public mumlib2::Callback {
public:
    mumlib2::Mumlib2 *mum;

    mumlib2::Logger logger = mumlib2::Logger("");


    virtual void audio(int target,
                       int sessionId,
                       int sequenceNumber,
                        bool is_last,
                       const int16_t *pcm_data,
                        size_t samples_count) override {
        logger.notice("Received audio: samples_count: %d", samples_count);
        mum->sendAudioData(pcm_data, samples_count);
    }

    virtual void textMessage(
            uint32_t actor,
            std::vector<uint32_t> session,
            std::vector<uint32_t> channel_id,
            std::vector<uint32_t> tree_id,
            std::string message) override {
        mumlib2::Callback::textMessage(actor, session, channel_id, tree_id, message);
        logger.notice("Received text message: %s", message.c_str());
        mum->sendTextMessage("someone said: " + message);
    }
};

int main(int argc, char *argv[]) {
    auto logger = mumlib2::Logger("");

    if (argc < 5) {
        logger.crit("Usage: %s {server} {port} {username} {password} [{certfile} {keyfile}]", argv[0]);
        return 1;
    }

    std::string server = argv[1];
    uint16_t port = std::stoi(argv[2]);
    std::string username = argv[3];
    std::string password = argv[4];

    MyCallback myCallback;
    while (true) {
        try {
            mumlib2::Mumlib2 mum(myCallback);
            myCallback.mum = &mum;
            mum.connect(server, port, username, password);
            mum.run();

        } catch (mumlib2::TransportException &exp) {
            logger.error("TransportException: %s.", exp.what());
            logger.notice("Attempting to reconnect in 5 s.");
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}
