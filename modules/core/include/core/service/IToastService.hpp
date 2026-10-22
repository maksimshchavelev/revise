// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/Notifiable.hpp> // for core::Notifiable

namespace core {

/// @brief Type of toast message
enum class ToastType {
    INFO,    ///< Informational toast
    SUCCESS, ///< Success toast
    ERROR    ///< Error toast
};

/// @brief A structure describing a single toast message
struct Toast {
    QString   header;  ///< Toast header
    QString   message; ///< Toast message
    ToastType type;    ///< Toast type

    bool operator==(const Toast& other) const {
        return header == other.header && message == other.message && type == other.type;
    }
};

/// @brief A service that manages toast messages
class IToastService : public Notifiable {
  public:
    EVENT(requested, Toast toast) ///< Toast was requested

    IToastService() = default;
    virtual ~IToastService() = default;

    /**
     * @brief Request toast
     * @param toast `Toast`
     */
    virtual void request(Toast toast) = 0;

    /// @brief Close the last toast
    virtual void close_last() = 0;
};

} // namespace core
