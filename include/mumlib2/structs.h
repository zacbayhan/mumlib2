// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <cstdint>
#include <string>

namespace mumlib2 {
    struct MumbleUser {
        int32_t sessionId = -1;
        int32_t channelId = -1;
        std::string name = "";

        bool local_mute = false;
    };

    struct MumbleChannel {
        int32_t channelId = -1;
        std::string name = "";
        std::string description = "";
    };
}
