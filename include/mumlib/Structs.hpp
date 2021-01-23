#pragma once

//stdlib
#include <cstdint>
#include <string>

namespace mumlib {
    struct MumbleUser {
        int32_t sessionId = -1;
        int32_t channelId = -1;
        std::string name = "";
    };

    struct MumbleChannel {
        int32_t channelId = -1;
        std::string name = "";
        std::string description = "";
    };
}
