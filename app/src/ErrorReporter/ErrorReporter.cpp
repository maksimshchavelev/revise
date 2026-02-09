// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Error reporter

#include <ErrorReporter/ErrorReporter.hpp>

#ifdef QT_DEBUG
    #include <boost/stacktrace.hpp>
#endif

// Public method
AppError::AppError(const QString&   message,
                   const QString&   details,
                   const QString&   source,
                   const QString&   stacktrace,
                   const QDateTime& timestamp,
                   QObject*         parent) :
    QObject(parent), m_message(message), m_details(details), m_source(source), m_stacktrace(stacktrace),
    m_timestamp(timestamp) {}

// Public method
const QString &AppError::message() const noexcept
{
    return m_message;
}

// Public method
const QString &AppError::details() const noexcept
{
    return m_details;
}

// Public method
const QString &AppError::source() const noexcept
{
    return m_source;
}

// Public method
const QString &AppError::stacktrace() const noexcept
{
    return m_stacktrace;
}

// Public method
const QDateTime &AppError::timestamp() const noexcept
{
    return m_timestamp;
}

namespace revise {

// Public method
ErrorReporter::ErrorReporter(QObject* parent) : QObject(parent) {
    qRegisterMetaType<AppError>("AppError");
}


// Public method
ErrorReporter* ErrorReporter::instance() {
    static ErrorReporter instance;
    return &instance;
}


// Public method
QString ErrorReporter::stacktrace() {
#ifdef QT_DEBUG
    auto extractLibraryName = [](const std::string& fullPath) {
        std::string result = fullPath;

        // Find the last slash in the path
        size_t lastSlash = result.find_last_of('/');
        if (lastSlash != std::string::npos) {
            result = result.substr(lastSlash + 1);
        }

        // Trim anything after .so extension
        size_t soPos = result.find(".so");
        if (soPos != std::string::npos) {
            result = result.substr(0, soPos + 3);
        }

        return result;
    };

    auto extractFunctionName = [](const QString& rawFrame) {
        QString functionName = rawFrame.trimmed();

        // Remove leading frame number if present (e.g., "0# ")
        if (functionName.contains('#')) {
            int hashPos = functionName.indexOf('#');
            if (hashPos != -1) {
                functionName = functionName.mid(hashPos + 1).trimmed();
            }
        }

        // Remove " in /path/to/lib.so" part if present
        int inPos = functionName.indexOf(" in ");
        if (inPos != -1) {
            functionName = functionName.left(inPos).trimmed();
        }

        return functionName;
    };

    const boost::stacktrace::stacktrace trace = boost::stacktrace::stacktrace();
    QStringList                         cleanedFrames;

    // Patterns to skip - frames related to stack trace collection itself
    static const QStringList skipPatterns = {
        "stacktrace()", "boost::stacktrace::stacktrace", "boost::stacktrace::basic_stacktrace", "getCleanStackTrace"};

    for (std::size_t i = 0; i < trace.size(); ++i) {
        const boost::stacktrace::frame frame = trace[i];

        // Get raw string representation
        const std::string rawFrameStr = boost::stacktrace::to_string(frame);
        if (rawFrameStr.empty()) {
            continue;
        }

        // Convert to QString for easier manipulation
        const QString frameStr = QString::fromStdString(rawFrameStr);

        // Check if this frame should be skipped
        bool shouldSkip = false;
        for (const QString& pattern : skipPatterns) {
            if (frameStr.contains(pattern)) {
                shouldSkip = true;
                break;
            }
        }

        if (shouldSkip) {
            continue;
        }

        QString functionName;
        QString libraryName;

        // Parse the frame string
        int inPos = frameStr.indexOf(" in ");

        if (inPos != -1) {
            // Extract and clean function name
            functionName = extractFunctionName(frameStr.left(inPos));

            // Extract and clean library path
            QString pathPart = frameStr.mid(inPos + 4).trimmed();
            libraryName = QString::fromStdString(extractLibraryName(pathPart.toStdString()));
        } else {
            // Frame doesn't follow the standard format
            functionName = extractFunctionName(frameStr);
            libraryName = "[unknown]";
        }

        // Format the cleaned frame
        QString cleanedFrame = QString("#%1 %2 [%3]").arg(cleanedFrames.size()).arg(functionName).arg(libraryName);

        cleanedFrames.append(cleanedFrame);
    }

    return cleanedFrames.join("\n");
#else
    return {};
#endif
}


// Public method
void ErrorReporter::report(const QString&   message,
                           const QString&   details,
                           const QString&   source,
                           const QString&   stacktrace,
                           const QDateTime& timestamp) {
    qWarning() << qPrintable(QString("[%1] %2: %3\nDetails: %4\nStacktrace:\n%5")
                                 .arg(timestamp.toString("hh:mm:ss::ms"))
                                 .arg(source)
                                 .arg(message)
                                 .arg(details)
                                 .arg(stacktrace));

    AppError* error = new AppError(message, details, source, stacktrace, timestamp, this);

    emit error_occured(error);
}



} // namespace revise
