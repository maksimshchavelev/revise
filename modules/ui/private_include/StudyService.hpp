// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                // for QObject
#include <core/IStudyService.hpp> // for core::IStudyService

namespace ui {

class StudyService : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(int timeLimit READ time_limit NOTIFY time_limit_changed FINAL)
    Q_PROPERTY(float timeRemaining READ time_remaining NOTIFY time_remaining_changed FINAL)
    Q_PROPERTY(QString front READ front NOTIFY front_changed FINAL)
    Q_PROPERTY(QString back READ back NOTIFY back_changed FINAL)
    Q_PROPERTY(bool flipped READ flipped NOTIFY flipped_changed FINAL)

    StudyService(core::IStudyService& study_service, QObject* parent = nullptr);

    /**
     * @brief Start training
     * @param deck_id Deck ID
     */
    Q_INVOKABLE void start(int deck_id);

    /**
     * @brief Answer card
     * @param difficulty Difficulty (0..5)
     */
    Q_INVOKABLE void answer(float difficulty);

    /**
     * @brief Abort training
     */
    Q_INVOKABLE void abort();

  signals:
    void time_limit_changed();
    void time_remaining_changed();
    void front_changed();
    void back_changed();
    void flipped_changed();

  private:
    core::IStudyService& m_study_service;

    int time_remaining() const;
    int time_limit() const;

    QString front() const;
    QString back() const;

    bool flipped() const;
};

} // namespace ui