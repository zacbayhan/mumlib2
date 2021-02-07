#pragma once

//stdlib
#include <cstdint>
#include <vector>

namespace mumlib {

    class VarInt {
    public:
        explicit VarInt(const uint8_t* buf);

        explicit VarInt(int8_t val);
        explicit VarInt(int16_t val);
        explicit VarInt(int32_t val);
        explicit VarInt(uint32_t val);
        explicit VarInt(int64_t val);

        [[nodiscard]] size_t Size() const;
        [[nodiscard]] size_t Value() const;

        [[nodiscard]] std::vector<uint8_t> Encode() const;
        
    private:
        void parse(const uint8_t* buf);

    private:
        int64_t _val = 0;
        size_t _size = 0;
    };
}
