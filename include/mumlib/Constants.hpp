#pragma once

namespace mumlib {
    constexpr uint32_t mumble_audio_channels   = 1;
    constexpr uint32_t mumble_audio_samplerate = 48000;

    constexpr int MAX_UDP_LENGTH = 1024;
    constexpr int MAX_TCP_LENGTH = 129 * 1024; // 128 kB + some reserve
}
