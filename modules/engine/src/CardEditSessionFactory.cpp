// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "engine/CardEditSessionFactory.hpp" // for header
#include "CardEditSession.hpp"               // for CardEditSession

namespace engine {

std::unique_ptr<core::ICardEditSession> create_card_edit_session([[maybe_unused]] CardEditSessionType type) {
    // TODO: don't forget add selection the type of edit session in the future
    return std::make_unique<CardEditSession>();
}

} // namespace engine
