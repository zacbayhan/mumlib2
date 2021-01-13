#pragma once

namespace mumlib {
    constexpr int DEFAULT_OPUS_ENCODER_BITRATE = 8000;
    constexpr int DEFAULT_OPUS_SAMPLE_RATE = 16000;
    constexpr int DEFAULT_OPUS_NUM_CHANNELS = 1;

    constexpr int MAX_UDP_LENGTH = 1024;
    constexpr int MAX_TCP_LENGTH = 129 * 1024; // 128 kB + some reserve
}
