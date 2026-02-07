// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>  // for QObject
#include <QThread>  // for QThread
#include <concepts> // for std::invocable

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
     * @brief Execute a function in the database thread **asynchronously**.
     *
     * @param fn Function to execute. It will be called in the database thread.
     * @param callback callback that will be called to pass the result of the request execution
     *
     * @note The function is executed via the Qt event queue using
     *       Qt::QueuedConnection semantics.
     */
    template <typename Result> void exec_async(std::function<Result()> fn, std::function<void(Result)> callback) {
        QMetaObject::invokeMethod(
            this, [fn = std::move(fn), callback = std::move(callback)]() { callback(fn()); }, Qt::QueuedConnection);
    }

    /**
     * @brief Specialization `DatabaseExecutionContext::dispatch_async` for type `void`
     */
    inline void exec_async(std::function<void()> fn, std::function<void()> callback) {
        QMetaObject::invokeMethod(
            this,
            [fn = std::move(fn), callback = std::move(callback)]() {
                fn();
                callback();
            },
            Qt::QueuedConnection);
    }

    /**
     * @brief Execute a function in the database thread **synchronously**.
     *
     * @param fn Function to execute.
     *
     * @return The method returns the same as the function passed to it.
     *
     * @note This call blocks the calling thread until execution completes.
     *       Use with care to avoid deadlocks.
     */
    template <typename Fn>
    auto exec(Fn&& fn) -> decltype(fn())
        requires std::invocable<Fn>
    {
        using Result = decltype(fn());

        // If we are already in the DB thread, we call the function directly. Without this check,
        // Qt::BlockingQueuedConnection will lead to a deadlock, as this flag blocks the calling thread.
        if (QThread::currentThread() == m_thread.currentThread()) {
            // void return type case
            if constexpr (std::is_void_v<Result>) {
                fn();
                return;
            } else {
                // non-void return type case
                return fn();
            }
        }

        // void return type case
        if constexpr (std::is_void_v<Result>) {
            QMetaObject::invokeMethod(this, [fn = std::forward<Fn>(fn)]() { fn(); }, Qt::BlockingQueuedConnection);
            return;
        } else {
            // non-void return type case
            Result result;

            QMetaObject::invokeMethod(
                this, [fn = std::forward<Fn>(fn), &result]() { result = fn(); }, Qt::BlockingQueuedConnection);

            return result;
        }
    }

  private:
    QThread m_thread; ///< For context
};

} // namespace io