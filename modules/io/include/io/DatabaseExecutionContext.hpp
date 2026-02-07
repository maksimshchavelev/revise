// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject> // for QObject
#include <QThread> // for QThread

namespace io {

/**
 * @brief Execution context that serializes all database-related code
 * into a single dedicated thread.
 *
 * @details This class owns a dedicated QThread and provides a safe mechanism
 * to execute arbitrary code in that thread via an event queue.
 *
 * The main purpose of DatabaseExecutionContext is to guarantee that
 * all interactions with thread-affine objects (such as QSqlDatabase
 * and QSqlQuery) are executed in the same thread in which they were created.
 *
 * This class:
 * - does NOT know anything about SQL or Database internals
 * - does NOT manage schemas or migrations
 * - does NOT own repositories or storages
 *
 * It only provides a strict execution boundary.
 *
 * Typical usage:
 * - Create DatabaseExecutionContext in application initialization code
 * - Create Database inside this context
 * - Route all database operations through this context
 *
 * This approach avoids:
 * - data races
 * - undefined behavior due to cross-thread QSqlDatabase usage
 * - manual mutex-based synchronization
 */
class DatabaseExecutionContext final : public QObject {
    Q_OBJECT

  public:
    /**
     * @brief Constructs the execution context and starts the database thread.
     *
     * @note The internal event loop is started immediately.
     */
    explicit DatabaseExecutionContext(QObject* parent = nullptr);

    /**
     * @brief Stops the database thread and waits for its completion.
     *
     * @note All queued tasks are completed before shutdown.
     */
    ~DatabaseExecutionContext() override;

    /**
     * @brief Execute a function in the database thread asynchronously.
     *
     * @param fn Function to execute. It will be called in the database thread.
     *
     * @note The function is executed via the Qt event queue using
     *       Qt::QueuedConnection semantics.
     */
    void post(std::function<void()> fn);

    /**
     * @brief Execute a function in the database thread synchronously.
     *
     * @param fn Function to execute.
     *
     * @note This call blocks the calling thread until execution completes.
     *       Use with care to avoid deadlocks.
     */
    void dispatch(std::function<void()> fn);

  private:
    QThread m_thread; ///< For context
};

} // namespace io