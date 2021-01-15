#pragma once

//stdlib
#include <cstdint>
#include <string>

namespace mumlib {
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