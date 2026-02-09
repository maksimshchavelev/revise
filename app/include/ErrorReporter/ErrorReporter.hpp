// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Error reporter

#pragma once

#include <QDateTime> // QDateTime
#include <QObject>   // for QObject
#include <QString>   // for QString

/**
 * @brief Application error. Used in conjunction with `ErrorReporter`.
 */
class AppError : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(QString message READ message CONSTANT)
    Q_PROPERTY(QString details READ details CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
    Q_PROPERTY(QString stacktrace READ stacktrace CONSTANT)
    Q_PROPERTY(QDateTime timestamp READ timestamp CONSTANT)

    AppError(const QString&   message,
             const QString&   details,
             const QString&   source,
             const QString&   stacktrace,
             const QDateTime& timestamp,
             QObject*         parent = nullptr);

    /**
     * @brief Get message
     * @return message
     */
    const QString& message() const noexcept;

    /**
     * @brief Get details
     * @return details
     */
    const QString& details() const noexcept;

    /**
     * @brief Get source
     * @return source
     */
    const QString& source() const noexcept;

    /**
     * @brief Get stacktrace
     * @return stacktrace
     */
    const QString& stacktrace() const noexcept;

    /**
     * @brief Get timestamp
     * @return timestamp
     */
    const QDateTime& timestamp() const noexcept;

private:
    QString   m_message;    ///< user-friendly message
    QString   m_details;    ///< technical details
    QString   m_source;     ///< e.g. "Database::get_decks"
    QString   m_stacktrace; ///< Stacktrace
    QDateTime m_timestamp;  ///< Timestamp
};

Q_DECLARE_METATYPE(AppError)

namespace revise {

/**
 * @brief Used for error logging
 */
class ErrorReporter : public QObject {
    Q_OBJECT

  public:
    explicit ErrorReporter(QObject* parent = nullptr);

    /**
     * @brief Get instance of `ErrorReporter`
     * @return pointer to `ErrorReporter` instance
     */
    static ErrorReporter* instance();

    /**
     * @brief Get stack trace
     * @return `QString` with stack trace
     */
    static QString stacktrace();

    /**
     * @brief Report error
     * @param message Message
     * @param details Details
     * @param source Where did the error occur? e.g. "Database::get_decks"
     * @param stacktrace Stacktrace (optional)
     * @param timestamp Timestamp (optional, current time by default)
     * @note Emit a signal `error_occured`
     */
    Q_INVOKABLE void report(const QString&   message,
                            const QString&   details,
                            const QString&   source,
                            const QString&   stacktrace = ErrorReporter::stacktrace(),
                            const QDateTime& timestamp = QDateTime::currentDateTime());

  signals:
    void error_occured(AppError* error);
};

} // namespace revise
