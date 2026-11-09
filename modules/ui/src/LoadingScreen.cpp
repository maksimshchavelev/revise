// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/LoadingScreen.hpp" // for header
#include <QMutexLocker>         // for QMutexLocker

namespace ui {

LoadingScreen::LoadingScreen(QObject* parent) : QObject(parent) {}


void LoadingScreen::set_visible(const bool visible) noexcept {
    {
        QMutexLocker _{&m_visible_mutex};
        m_visible = visible;
    }

    QMetaObject::invokeMethod(this, [this] {
        emit visibleChanged();
    });
}


bool LoadingScreen::visible() const noexcept {
    QMutexLocker _{&m_visible_mutex};
    return m_visible;
}


void LoadingScreen::set_description(QString description) {
    {
        QMutexLocker _{&m_description_mutex};
        m_description = std::move(description);
    }

    QMetaObject::invokeMethod(this, [this] {
        emit descriptionChanged();
    });
}


QString LoadingScreen::description() const {
    QMutexLocker _{&m_description_mutex};
    return m_description;
}

} // namespace ui
