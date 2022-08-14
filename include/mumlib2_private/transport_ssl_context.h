// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <string>

//boost
#include <asio.hpp>
#include <asio/ssl.hpp>

namespace mumlib2{

    /* This helper is needed because the sslContext and sslSocket are initialized in
     * the Transport constructor and there wasn't an easier way of passing these two
     * arguments.
     * TODO: add support for password callback.
     */
    class SslContextHelper {
    public:
        //mark as non-copyable
        SslContextHelper(const SslContextHelper&) = delete;
        SslContextHelper& operator=(const SslContextHelper&) = delete;

        SslContextHelper(asio::ssl::context &ctx,
                         std::string cert_file, std::string privkey_file) {
            if (cert_file.size() > 0) {
                ctx.use_certificate_file(cert_file, asio::ssl::context::file_format::pem);
            }
            if (privkey_file.size() > 0) {
                ctx.use_private_key_file(privkey_file, asio::ssl::context::file_format::pem);
            }
        }
        ~SslContextHelper() = default;
    };
}