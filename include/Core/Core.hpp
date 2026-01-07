// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#pragma once

#include <Database/Database.hpp>                   // for Database
#include <DecksModel/DecksModel.hpp>               // for DecksModel
#include <CardsModel/CardsModel.hpp>               // for CardsModel
#include <StreakService/StreakService.hpp>         // for StreakService
#include <DeckImporter/AnkiDeckImporter.hpp>       // for DeckImporter
#include <SM2Algorithm/SM2Algorithm.hpp>           // for SM2
#include <SqlDeckRepository/SqlDeckRepository.hpp> // for SqlDeckRepository
#include <StudyService/StudyService.hpp>           // for StudyService
#include <DeckService/DeckService.hpp>             // for DeckService
#include <QGuiApplication>                         // for QGuiApplication
#include <QObject>                                 // for QObject
#include <DeckMediaStorage/DeckMediaStorage.hpp>   // for DeckMediaStorage
#include <HtmlHelper/HtmlHelper.hpp>               // for HtmlHelper
#include <MathJaxRenderer/MathJaxRenderer.hpp>     // for MathJaxRenderer
#include <DeckExporter/ReviseDeckExporter.hpp>     // for ReviseDeckExporter

namespace revise {

class Core : public QObject {
    Q_OBJECT

  public:
    Core(QGuiApplication& app, QObject* parent = nullptr);

    int run();

  private:
    QGuiApplication&   m_app;
    Database           m_db;
    DeckMediaStorage   m_deck_media_storage;
    HtmlHelper         m_html_helper;
    SqlDeckRepository  m_sql_deck_repo;
    SM2Algorithm       m_sm2_algo;
    StreakService      m_streak_service;
    StudyService       m_study_service;
    AnkiDeckImporter   m_anki_importer;
    ReviseDeckExporter m_revise_exporter;
    DeckService        m_deck_service;
    DecksModel         m_decks_model;
    CardsModel         m_cards_model;
    MathJaxRenderer    m_mathjax_renderer;

    // Helper
    std::unique_ptr<IDeckRepository> make_thread_local_sql_repo(const QString& conn_name);

    // Helper
    void request_permission_if_not_granted(const QString& permission);
};

} // namespace revise
