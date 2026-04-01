// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/ICardEditSession.hpp> // for core::ICardEditSession
#include <memory>                    // for std::unique_ptr

namespace engine {

enum class CardEditSessionType {
    Local ///< Edit session over local card
};

/**
 * @brief The function creates a specific implementation of card edit session and returns a pointer to it.
 * @type type Type of card edit session
 * @return `std::unique_ptr<core::ICardEditSession>`
 */
std::unique_ptr<core::ICardEditSession> create_card_edit_session(CardEditSessionType type);

} // namespace engine
