// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <core/IUpdateChecker.hpp> // for core::IUpdateChecker.hpp
#include <gtest/gtest.h>           // for gtest

using namespace core;

TEST(IUpdateChecker_test, create_network_unavailable_error) {
    const auto [kind, message] = IUpdateChecker::Error::network_unavailable("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::NetworkUnavailable);
    EXPECT_EQ(message, "error");
}


TEST(IUpdateChecker_test, create_timeout_error) {
    const auto [kind, message] = IUpdateChecker::Error::timeout("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::Timeout);
    EXPECT_EQ(message, "error");
}


TEST(IUpdateChecker_test, create_not_found_error) {
    const auto [kind, message] = IUpdateChecker::Error::not_found("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::NotFound);
    EXPECT_EQ(message, "error");
}


TEST(IUpdateChecker_test, create_server_error) {
    const auto [kind, message] = IUpdateChecker::Error::server_error("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::ServerError);
    EXPECT_EQ(message, "error");
}


TEST(IUpdateChecker_test, create_parsing_error) {
    const auto [kind, message] = IUpdateChecker::Error::parsing_error("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::ParsingError);
    EXPECT_EQ(message, "error");
}


TEST(IUpdateChecker_test, create_internal_error) {
    const auto [kind, message] = IUpdateChecker::Error::internal_error("error");

    EXPECT_EQ(kind, IUpdateChecker::ErrorKind::InternalError);
    EXPECT_EQ(message, "error");
}
