// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#define PROPERTY(public_name, private_member)                                                                          \
  public:                                                                                                              \
    const auto& public_name() const noexcept {                                                                         \
        return private_member;                                                                                         \
    }                                                                                                                  \
    void set_##public_name(const auto& value) {                                                                        \
        private_member = value;                                                                                        \
    }