#pragma once

//stdlib
#include <cstdint>
#include <string>

namespace mumlib {
    struct MumlibConfiguration {
        int opusEncoderBitrate = 0;
        std::string cert_file = "";
        std::string privkey_file = "";
    };

    struct MumbleUser {
        int32_t sessionId;
        std::string name;
    };

    struct MumbleChannel {
        int32_t channelId;
        std::string name;
        std::string description;
    };
}