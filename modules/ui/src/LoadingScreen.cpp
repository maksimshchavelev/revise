// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/LoadingScreen.hpp" // for header

namespace ui {

LoadingScreen::LoadingScreen(QObject* parent) : QObject(parent) {}

void LoadingScreen::set_visible(bool visible) noexcept {
    m_visible = visible;
    emit visibleChanged();
}

bool LoadingScreen::visible() const noexcept {
    return m_visible;
}

} // namespace ui
