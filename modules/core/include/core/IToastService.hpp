// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Notifiable.hpp" // for Notifiable
#include <QString>        // for QString

namespace core {

enum class ToastType {
    INFO,    ///< Info toast
    SUCCESS, ///< Success toast
    ERROR    ///< Error toast
};

/**
 * @brief A structure describing a single toast message
 */
struct Toast {
    QString   header;  ///< Toast header
    QString   message; ///< Toast message
    ToastType type;    ///< Toast type
};

/**
 * @brief The IToastService class
 */
class IToastService : public Notifiable {
  public:
    EVENT(requested, Toast toast) ///< Toast requested

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
