// Copyright (C) 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>                       // for gtest
#include <network/RequestExecutionContext.hpp> // for RequestExecutionContext
#include <thread>

using namespace network;


namespace {
class RequestExecutionContextTest : public testing::Test {
  protected:
    RequestExecutionContext context{};
};
} // namespace


TEST_F(RequestExecutionContextTest, can_execute_void_function) {
    context.exec([](QNetworkAccessManager&) {});
}


TEST_F(RequestExecutionContextTest, can_execute_function_that_returns_value) {
    const auto value = context.exec([](QNetworkAccessManager&) { return 42; });
    EXPECT_EQ(value, 42);
}


TEST_F(RequestExecutionContextTest, call_exec_from_other_thread) {
    // If `this_thread` (the parent thread) is not equal to `other_thread1` or `other_thread2`, but `other_thread1` is equal to
    // `other_thread2`, then `exec` is executed in its own thread, as intended
    // This test checks the `exec` call from the parent thread and from another thread

    const auto                   this_thread = std::this_thread::get_id();
    std::thread::id              other_thread1;
    std::atomic<std::thread::id> other_thread2;

    context.exec([&](QNetworkAccessManager&) { other_thread1 = std::this_thread::get_id(); });
    std::jthread t([&] { context.exec([&](QNetworkAccessManager&) { other_thread2.store(std::this_thread::get_id()); }); });

    t.join();

    EXPECT_NE(this_thread, other_thread1);
    EXPECT_NE(this_thread, other_thread2);
    EXPECT_EQ(other_thread1, other_thread2);
}
