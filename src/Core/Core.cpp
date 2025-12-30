// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#include <Core/Core.hpp>                               // for Core
#include <ErrorReporter/ErrorReporter.hpp>             // for ErrorReporter
#include <NotificationManager/NotificationManager.hpp> // for NotificationManager
#include <QQmlApplicationEngine>                       // for QQmlApplicationEngine
#include <QQmlContext>                                 // for QQmlContext
#include <QtCore/private/qandroidextras_p.h>           // for QtAndroidPrivate

namespace revise {

// Public method
Core::Core(QGuiApplication& app, QObject* parent) :
    QObject(parent), m_app(app), m_db("default", this), m_streak_backend(m_db), m_study_controller(m_db),
    m_decks_model(m_db, m_study_controller), m_decks_io() {

    // Update streak and decks model when training finished
    connect(&m_study_controller, &StudyController::training_finished, this, [this]() {
        if (!m_streak_backend.is_updated_today()) {
            m_streak_backend.set_streak(m_streak_backend.streak() + 1);
        }

        m_decks_model.update();
    });

    connect(&m_decks_io, &DecksIO::import_finished, this, [this](int) { m_decks_model.update(); });

    auto permission_check_future = QtAndroidPrivate::checkPermission("android.permission.POST_NOTIFICATIONS");
    auto permission_check_result = permission_check_future.result(); // wait for result

    if (permission_check_result != QtAndroidPrivate::PermissionResult::Authorized) {
        // if not authorized, request permission
        [[maybe_unused]] auto permission_request_result =
            QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS").result();
    }

    QVector<QString> notifications = {
        "Пора что нибудь повторить!",
        "Ты целых два дня ничего не повторял",
        "Ты так долго ничего не повторял...",
        "Найдешь 5 минут для повторения?",
        "Давай повторим карточки!",
        "Нужно повторить карточки",
        "Не хочешь кое что повторить?"
    };

    NotificationManager::clear_all_scheduled_notifications();

    // weekly shedule
    for (int i = 0; i < 7; ++i) {
        int base_id = qAbs(qHash(QDateTime::currentDateTime().toMSecsSinceEpoch())) % 1000000;
        QDateTime time = QDateTime::currentDateTime().addDays(i + 1);
        time.setTime(QTime(12, 00, 0)); // 12::00::00 by the local time
        NotificationManager::schedule_notification(notifications[i], time, base_id);
    }
}


// Public method
int Core::run() {
    // Reset streak if overdue
    m_streak_backend.reset_streak_if_overdue();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("streakBackend", &m_streak_backend);
    engine.rootContext()->setContextProperty("decksModel", &m_decks_model);
    engine.rootContext()->setContextProperty("studyController", &m_study_controller);
    engine.rootContext()->setContextProperty("decksIO", &m_decks_io);
    engine.rootContext()->setContextProperty("errorReporter", ErrorReporter::instance());

    qmlRegisterUncreatableType<AppError>(
        "Revise", 1, 0, "AppError", "AppError is only available via the ErrorReporter");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &m_app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("revise", "Main");

    return m_app.exec();
}


} // namespace revise
