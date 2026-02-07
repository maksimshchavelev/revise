// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "io/DatabaseExecutionContext.hpp" // for header

namespace io {

DatabaseExecutionContext::DatabaseExecutionContext(QObject* parent) : QObject(parent) {
    m_thread.start();

    // Move yourself to thread (context). Now this class lives in this thread, even if it was created in another one.
    // This is necessary for the functions to execute correctly in this thread.
    this->moveToThread(&m_thread);
}

DatabaseExecutionContext::~DatabaseExecutionContext() {
    m_thread.quit();
    m_thread.wait();
}

void DatabaseExecutionContext::post(std::function<void()> fn) {
    QMetaObject::invokeMethod(this, [fn = std::move(fn)]() { fn(); }, Qt::QueuedConnection);
}

void DatabaseExecutionContext::dispatch(std::function<void()> fn) {
    // If we are already in the DB thread, we call the function directly. Without this check,
    // Qt::BlockingQueuedConnection will lead to a deadlock, as this flag blocks the calling thread.
    if (QThread::currentThread() == m_thread.currentThread()) {
        fn();
        return;
    }

    QMetaObject::invokeMethod(this, [fn = std::move(fn)]() { fn(); }, Qt::BlockingQueuedConnection);
}

} // namespace io
