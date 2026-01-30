// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#pragma once

#include <CardsModel/CardsModel.hpp>               // for CardsModel
#include <Database/Database.hpp>                   // for Database
#include <DeckExporter/ReviseDeckExporter.hpp>     // for ReviseDeckExporter
#include <DeckImporter/AnkiDeckImporter.hpp>       // for DeckImporter
#include <DeckImporter/ReviseDeckImporter.hpp>     // for ReviseDeckImporter
#include <DeckMediaStorage/DeckMediaStorage.hpp>   // for DeckMediaStorage
#include <DeckService/DeckService.hpp>             // for DeckService
#include <DecksModel/DecksModel.hpp>               // for DecksModel
#include <HtmlHelper/HtmlHelper.hpp>               // for HtmlHelper
#include <QGuiApplication>                         // for QGuiApplication
#include <QObject>                                 // for QObject
#include <SM2Algorithm/SM2Algorithm.hpp>           // for SM2
#include <SqlDeckRepository/SqlDeckRepository.hpp> // for SqlDeckRepository
#include <SqlEventRecorder/SqlEventRecorder.hpp>   // for SqlEventRecorder
#include <StreakService/StreakService.hpp>         // for StreakService
#include <StudyService/StudyService.hpp>           // for StudyService
#include <core/INotificationService.hpp>           // for INotificationService
#include <core/IPermissionService.hpp>             // for IPermissionService

namespace revise {

class Core : public QObject {
    Q_OBJECT

  public:
    Core(QGuiApplication& app, QObject* parent = nullptr);

    int run();

  private:
    QGuiApplication&   m_app;
    Database           m_db;
    SqlEventRecorder   m_sql_event_recorder;
    DeckMediaStorage   m_deck_media_storage;
    HtmlHelper         m_html_helper;
    SqlDeckRepository  m_sql_deck_repo;
    SM2Algorithm       m_sm2_algo;
    StreakService      m_streak_service;
    StudyService       m_study_service;
    AnkiDeckImporter   m_anki_importer;
    ReviseDeckImporter m_revise_importer;
    ReviseDeckExporter m_revise_exporter;
    DeckService        m_deck_service;
    DecksModel         m_decks_model;
    CardsModel         m_cards_model;

    std::unique_ptr<core::IPermissionService>   m_permission_service;
    std::unique_ptr<core::INotificationService> m_notification_service;

    // Helper
    std::unique_ptr<IDeckRepository> make_thread_local_sql_repo(const QString& conn_name);

    // Helper
    void request_permission_if_not_granted(const QString& permission);

    /**
     * @brief Copies HTML assets from resources to AppData
     */
    void extract_web_bundle() const;

    /**
     * @brief Schedules notifications (weekly)
     * @param today_enabled If `true`, the notification will be scheduled for today.
     * @param at When to show notifications
     * @note If the user has completed the training, notifications should be scheduled without taking today into
     * account, so set `today_enabled` = `false`.
     * @note The method clears notifications that have already been scheduled.
     */
    void schedule_notifications(bool today_enabled = true, const QTime& at = QTime(10, 0, 0)) const;
};

} // namespace revise
