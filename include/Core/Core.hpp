// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// Core of the application

#pragma once

#include <Database/Database.hpp>               // for Database
#include <DecksIO/DecksIO.hpp>                 // for DecksIO
#include <DecksModel/DecksModel.hpp>           // for DecksModel
#include <QGuiApplication>                     // for QGuiApplication
#include <QObject>                             // for QObject
#include <StreakBackend/StreakBackend.hpp>     // for StreakBackend
#include <StudyController/StudyController.hpp> // for StudyController

namespace revise {

class Core : public QObject {
    Q_OBJECT

  public:
    Core(QGuiApplication& app, QObject* parent = nullptr);

    int run();

  private:
    QGuiApplication& m_app;
    Database         m_db;
    StreakBackend    m_streak_backend;
    StudyController  m_study_controller;
    DecksModel       m_decks_model;
    DecksIO          m_decks_io;
};

} // namespace revise
