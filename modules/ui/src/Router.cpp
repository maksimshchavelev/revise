// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/Router.hpp" // for header
#include <platform/Platform.hpp> // for Platform::supports_multiple_windows

namespace ui {

void Router::push_page(QString name, QQmlComponent* page) {
    m_pages[name] = page;
}


void Router::navigate(Page page) {
    if (!platform::Platform::supports_multiple_windows()) {
        page.mode = Page::OpenMode::Page;
    }

    m_history.push_back(std::move(page));
    emit pageChanged();
}


void Router::navigate(QString page_name, QVariantMap params, Page::OpenMode mode) {
    navigate(Page{std::move(page_name), std::move(params), mode});
}


void Router::back() {
    if (m_history.size() > 1) {
        m_history.pop_back();
        emit pageChanged();
    }
}


QQmlComponent* Router::current_page_component() {
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
