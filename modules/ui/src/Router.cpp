// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/Router.hpp"         // for header
#include <platform/Platform.hpp> // for Platform::supports_multiple_windows

namespace ui {

void Router::push_page(QString name, QQmlComponent* page) {
    QObject* page_object = page->create();

    if (page->isError()) {
        qWarning() << "Failed to create QQuickItem from page with name" << name;
        return;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(page_object);

    if (!item) {
        qWarning() << "Failed to cast page_object to QQuickItem for page with name" << name;
        return;
    }

    m_pages[name] = item;
}


void Router::navigate(Page page) {
    if (!m_history.empty() && page == m_history.back()) {
        return;
    }

    if (!platform::Platform::supports_multiple_windows()) {
        page.mode = Page::OpenMode::Page;
    }

    QQuickItem* previous = current_page_item();

    if (previous) {
        previous->setVisible(false);
        previous->setParentItem(nullptr);

        QMetaObject::invokeMethod(previous, "onExit", Qt::DirectConnection);
    }

    QMetaObject::invokeMethod(m_pages[page.name], "onEnter", Qt::DirectConnection);
    m_pages[page.name]->setVisible(true);
    m_history.push_back(std::move(page));
    emit pageChanged();
}


void Router::navigate(QString page_name, QVariantMap params, Page::OpenMode mode) {
    navigate(Page{std::move(page_name), std::move(params), mode});
}


void Router::back() {
    if (m_history.size() > 1) {
        QQuickItem* previous = current_page_item();

        previous->setVisible(false);
        previous->setParentItem(nullptr);

        QMetaObject::invokeMethod(previous, "onExit", Qt::DirectConnection);

        m_history.pop_back();

        QQuickItem* current = current_page_item();

        current->setVisible(true);
        current->setParentItem(nullptr);

        QMetaObject::invokeMethod(previous, "onEnter", Qt::DirectConnection);

        emit pageChanged();
    }
}


QQuickItem* Router::current_page_item() {
    if (m_history.empty()) {
        return nullptr;
    }

    return m_pages[m_history.last().name];
}


Page Router::current_page() const {
    if (m_history.empty()) {
        return {};
    }

    return m_history.last();
}

} // namespace ui
