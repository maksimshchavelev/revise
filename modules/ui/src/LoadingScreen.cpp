// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/LoadingScreen.hpp" // for header
#include <QMutexLocker>         // for QMutexLocker

namespace ui {

LoadingScreen::LoadingScreen(QObject* parent) : QObject(parent) {}

void LoadingScreen::set_visible(bool visible) noexcept {
    m_visible = visible;
    emit visibleChanged();
}

bool LoadingScreen::visible() const noexcept {
    return m_visible;
}

void LoadingScreen::set_description(QString description) {
    {
        QMutexLocker _(&m_mutex);
        m_description = std::move(description);
    }

    emit descriptionChanged();
}

QString LoadingScreen::description() const {
    QMutexLocker _(&m_mutex);
    return m_description;
}

} // namespace ui
