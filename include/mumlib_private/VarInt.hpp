#pragma once

//stdlib
#include <cstdint>
#include <vector>
#include <string>

//mumlib
#include <mumlib.hpp>

namespace mumlib {

    class VarInt {
    public:
        VarInt(uint8_t *encoded);

        VarInt(std::vector<uint8_t> encoded);

        VarInt(int64_t value);

        int64_t getValue() const {
            return this->value;
        }

        std::vector<uint8_t> getEncoded() const;

    private:
        const int64_t value;

        int64_t parseVariant(const uint8_t *buffer);
    };
}