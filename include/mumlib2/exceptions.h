// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <stdexcept>
#include <string>

namespace mumlib2 {
    class Mumlib2Exception : public std::runtime_error {
    public:
        Mumlib2Exception(std::string message) : std::runtime_error(message) { }
    };

    class AudioDecoderException : public Mumlib2Exception {
    public:
        explicit AudioDecoderException(std::string message) : Mumlib2Exception(message) { }
    };

    class AudioEncoderException : public Mumlib2Exception {
    public:
        explicit AudioEncoderException(std::string message) : Mumlib2Exception(message) { }
    };

    class AudioPacketException : public Mumlib2Exception {
    public:
        explicit AudioPacketException(std::string message) : Mumlib2Exception(message) { }
    };

    class TransportException : public Mumlib2Exception {
    public:
        TransportException(std::string message) : Mumlib2Exception(std::move(message)) { }
    };

    class VarIntException : public Mumlib2Exception {
    public:
        VarIntException(std::string message) : Mumlib2Exception(message) { }
    };
}
