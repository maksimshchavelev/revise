// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "Notifiable.hpp" // for Notifiable
#include <QString>        // for QString

namespace core {

/**
 * @brief A class that implements the card editing session interface
 *
 * Why is it needed? For example, a user edits a card by adding or removing images from it. When they finish editing,
 * they want to save the changes. In this case, the session implementation will track which images were removed and
 * which were added, and synchronize the changes with the database.
 */
class ICardEditSession : public Notifiable {
  public:
    EVENT(editing_finished, QString front; QString back) ///< Editing finished

    ICardEditSession() = default;
    virtual ~ICardEditSession() = default;

    /**
     * @brief Begin editing session
     * @param front Card front
     * @param back Card back
     */
    virtual void begin(QString front, QString back) = 0;

    /**
     * @brief End editing session
     */
    virtual void end() = 0;

    /**
     * @brief Set card front
     * @param front Card front
     */
    virtual void set_front(QString front) = 0;

    /**
     * @brief Get card front
     * @return Card front
     */
    virtual QString front() const = 0;

    /**
     * @brief Set card back
     * @param back Card back
     */
    virtual void set_back(QString back) = 0;

    /**
     * @brief Get card back
     * @return Card back
     */
    virtual QString back() const = 0;
};

} // namespace core
