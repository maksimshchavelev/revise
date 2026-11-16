// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTemporaryDir>   // for QTemporaryDir
#include <gtest/gtest.h>   // for gtest
#include <io/Settings.hpp> // for Settings

using namespace io;

class SettingsTest : public testing::Test {
  protected:
    void SetUp() override {
        ASSERT_TRUE(m_dir.isValid());
        QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, m_dir.path());
        QSettings::setDefaultFormat(QSettings::IniFormat);
    }

    void TearDown() override {}

  private:
    QTemporaryDir m_dir; ///< Test directory
};

// ================================ TESTS ================================

// -------------------------- Default values --------------------------
TEST_F(SettingsTest, default_learning_rate) {
    const Settings settings{};
    EXPECT_FLOAT_EQ(settings.learning_rate(), 2.5f);
}


TEST_F(SettingsTest, default_max_interval) {
    const Settings settings{};
    EXPECT_EQ(settings.max_interval(), 14);
}


TEST_F(SettingsTest, default_ui_scale) {
    const Settings settings{};
    EXPECT_FLOAT_EQ(settings.ui_scale(), 1.0f);
}


TEST_F(SettingsTest, default_notifications) {
    const Settings settings{};
    EXPECT_TRUE(settings.notify_about_early_access());
    EXPECT_TRUE(settings.notify_about_too_old_android());
}


TEST_F(SettingsTest, default_dialogs) {
    const Settings settings{};
    EXPECT_TRUE(settings.ask_abort_learning());
    EXPECT_TRUE(settings.ask_relearn_many_cards());
    EXPECT_TRUE(settings.ask_relearn_one_card());
    EXPECT_TRUE(settings.ask_remove_deck());
    EXPECT_TRUE(settings.ask_remove_many_cards());
    EXPECT_TRUE(settings.ask_remove_one_card());
    EXPECT_TRUE(settings.ask_set_difficulty_for_many_cards());
    EXPECT_TRUE(settings.ask_set_state_for_many_cards());
}


TEST_F(SettingsTest, default_use_mobile_ui) {
#ifndef Q_OS_ANDROID
    const Settings settings{};
    EXPECT_FALSE(settings.use_mobile_ui());
#endif
}


// -------------------------- Overwrite Settings --------------------------
TEST_F(SettingsTest, overwrite_learning_rate) {
    Settings settings{};
    const auto result = settings.set_learning_rate(3.5f);
    ASSERT_TRUE(result);
    EXPECT_FLOAT_EQ(settings.learning_rate(), 3.5f);
}


TEST_F(SettingsTest, overwrite_max_interval) {
    Settings settings{};
    const auto result = settings.set_max_interval(30);
    ASSERT_TRUE(result);
    EXPECT_EQ(settings.max_interval(), 30);
}


TEST_F(SettingsTest, overwrite_ui_scale) {
    Settings settings{};
    const auto result = settings.set_ui_scale(1.2f);
    ASSERT_TRUE(result);
    EXPECT_FLOAT_EQ(settings.ui_scale(), 1.2f);
}


TEST_F(SettingsTest, overwrite_notifications) {
    Settings settings{};

    ASSERT_TRUE(settings.set_notify_about_early_access(false));
    ASSERT_TRUE(settings.set_notify_about_too_old_android(false));

    EXPECT_FALSE(settings.notify_about_early_access());
    EXPECT_FALSE(settings.notify_about_too_old_android());
}


TEST_F(SettingsTest, overwrite_dialogs) {
    Settings settings{};

    ASSERT_TRUE(settings.set_ask_abort_learning(false));
    ASSERT_TRUE(settings.set_ask_relearn_many_cards(false));
    ASSERT_TRUE(settings.set_ask_relearn_one_card(false));
    ASSERT_TRUE(settings.set_ask_remove_deck(false));
    ASSERT_TRUE(settings.set_ask_remove_many_cards(false));
    ASSERT_TRUE(settings.set_ask_remove_one_card(false));
    ASSERT_TRUE(settings.set_ask_set_difficulty_for_many_cards(false));
    ASSERT_TRUE(settings.set_ask_set_state_for_many_cards(false));

    EXPECT_FALSE(settings.ask_abort_learning());
    EXPECT_FALSE(settings.ask_relearn_many_cards());
    EXPECT_FALSE(settings.ask_relearn_one_card());
    EXPECT_FALSE(settings.ask_remove_deck());
    EXPECT_FALSE(settings.ask_remove_many_cards());
    EXPECT_FALSE(settings.ask_remove_one_card());
    EXPECT_FALSE(settings.ask_set_difficulty_for_many_cards());
    EXPECT_FALSE(settings.ask_set_state_for_many_cards());
}


TEST_F(SettingsTest, overwrite_use_mobile_ui) {
    Settings settings{};
    const auto result = settings.set_use_mobile_ui(true);
    ASSERT_TRUE(result);
    EXPECT_TRUE(settings.use_mobile_ui());
}


// -------------------------- Overwrite with incorrect values --------------------------
TEST_F(SettingsTest, cannot_set_learning_rate_below_bounds) {
    Settings settings{};
    const auto result = settings.set_learning_rate(0.5f);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_FLOAT_EQ(settings.learning_rate(), 2.5f);
}


TEST_F(SettingsTest, cannot_set_learning_rate_above_bounds) {
    Settings settings{};
    const auto result = settings.set_learning_rate(5.5f);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_FLOAT_EQ(settings.learning_rate(), 2.5f);
}


TEST_F(SettingsTest, cannot_set_max_interval_below_bounds) {
    Settings settings{};
    const auto result = settings.set_max_interval(0);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_EQ(settings.max_interval(), 14);
}


TEST_F(SettingsTest, cannot_set_max_interval_above_bounds) {
    Settings settings{};
    const auto result = settings.set_max_interval(366);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_EQ(settings.max_interval(), 14);
}


TEST_F(SettingsTest, cannot_set_ui_scale_below_bounds) {
    Settings settings{};
    const auto result = settings.set_ui_scale(0.6f);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_FLOAT_EQ(settings.ui_scale(), 1.0f);
}


TEST_F(SettingsTest, cannot_set_ui_scale_above_bounds) {
    Settings settings{};
    const auto result = settings.set_ui_scale(1.4f);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error().kind, core::SettingsError::Kind::OutOfBounds);
    EXPECT_FLOAT_EQ(settings.ui_scale(), 1.0f);
}
