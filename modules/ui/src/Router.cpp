// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/Router.hpp"         // for header
#include <QThread>               // for QThread
#include <platform/Platform.hpp> // for Platform::supports_multiple_windows

namespace ui {

void Router::push_page(const QString& name, QQmlComponent* page) {
    if (m_pages.contains(name)) {
        return;
    }

    auto incubator = std::make_shared<QQmlIncubator>();
    m_pages.insert(name, incubator);
    page->create(*m_pages[name]);
}


void Router::navigate(Page page) {
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(
            this, [this, page = std::move(page)] { navigate(std::move(page)); }, Qt::QueuedConnection);
        return;
    }

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

    QQuickItem* current_page = this->page(page.name);

    if (!current_page) {
        qWarning() << "this->page(page.name) returned nullptr";
        return;
    }

    QMetaObject::invokeMethod(current_page, "onEnter", Qt::DirectConnection);
    current_page->setVisible(true);
    m_history.push_back(std::move(page));
    emit pageChanged();
}


void Router::navigate(QString page_name, QVariantMap params, Page::OpenMode mode) {
    navigate(Page{std::move(page_name), std::move(params), mode});
}


void Router::back() {
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, [this]() { back(); }, Qt::QueuedConnection);
        return;
    }

    if (m_history.size() > 1) {
        QQuickItem* previous = current_page_item();

        previous->setVisible(false);
        previous->setParentItem(nullptr);

        QMetaObject::invokeMethod(previous, "onExit", Qt::DirectConnection);

        m_history.pop_back();

        QQuickItem* current = current_page_item();

        if (!current) {
            qWarning() << "Router::back(): failed to get current page item. Got nullptr";
            return;
        }

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

    return page(m_history.last().name);
}


Page Router::current_page() const {
    if (m_history.empty()) {
        return {};
    }

    return m_history.last();
}


QQuickItem* Router::page(const QString& name) {
    if (m_pages[name]->isError()) {
        qWarning() << "Failed to create page via incubator:" << m_pages[name]->errors();
        return nullptr;
    }

    if (m_pages[name]->isLoading()) {
        m_pages[name]->forceCompletion();
    }

    if (m_pages[name]->isReady()) {
        QObject* page_object = m_pages[name]->object();
        return qobject_cast<QQuickItem*>(page_object);
    }

    qWarning() << "Unknown error in Router::page()";
    return nullptr;
}

} // namespace ui
