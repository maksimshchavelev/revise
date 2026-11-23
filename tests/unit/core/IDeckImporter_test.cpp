// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <core/IDeckImporter.hpp> // for core::IDeckImporter
#include <gtest/gtest.h>          // for gtest.h

using namespace core;

TEST(IDeckImporter_test, create_internal_error) {
    const auto [kind, message] = IDeckImporter::Error::internal("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::InternalError);
}


TEST(IDeckImporter_test, create_unknown_format_error) {
    const auto [kind, message] = IDeckImporter::Error::unknown_format("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::UnknownFormat);
}


TEST(IDeckImporter_test, create_file_corrupted_error) {
    const auto [kind, message] = IDeckImporter::Error::file_corrupted("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::FileCorrupted);
}


TEST(IDeckImporter_test, create_extraction_failed) {
    const auto [kind, message] = IDeckImporter::Error::extraction_failed("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::ExtractionFailed);
}


TEST(IDeckImporter_test, create_not_supported_error) {
    const auto [kind, message] = IDeckImporter::Error::not_supported("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::NotSupported);
}


TEST(IDeckImporter_test, create_failed_to_open_file_error) {
    const auto [kind, message] = IDeckImporter::Error::failed_to_open_file("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::FailedToOpenFile);
}


TEST(IDeckImporter_test, create_outdated_format_error) {
    const auto [kind, message] = IDeckImporter::Error::outdated_format("error description");

    EXPECT_EQ(message, "error description");
    EXPECT_EQ(kind, IDeckImporter::ErrorKind::OutdatedFormat);
}
