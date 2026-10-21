// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "wrappers/CardWrapper.hpp"           // for Card
#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/IStudyService.hpp>             // for core::IStudyService

namespace ui {

/// UI wrapper over `core::StudyService`
class StudyService : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("StudyService is provided by C++")

  public:
    Q_PROPERTY(int timeLimit READ time_limit NOTIFY time_limit_changed FINAL)
    Q_PROPERTY(float timeRemaining READ time_remaining NOTIFY time_remaining_changed FINAL)
    Q_PROPERTY(QString front READ front NOTIFY front_changed FINAL)
    Q_PROPERTY(QString back READ back NOTIFY back_changed FINAL)
    Q_PROPERTY(bool flipped READ flipped NOTIFY flipped_changed FINAL)
    Q_PROPERTY(Card card READ card NOTIFY card_changed FINAL)

    explicit StudyService(core::IStudyService& study_service, QObject* parent = nullptr);

    /**
     * @brief Start training
     * @param deck_id Deck ID
     */
    Q_INVOKABLE void start(int deck_id) const;

    /**
     * @brief Answer card
     * @param difficulty Difficulty (0..5)
     */
    Q_INVOKABLE void answer(float difficulty) const;

    /// @brief Abort training
    Q_INVOKABLE void abort() const;

    /// @brief Flip the card
    Q_INVOKABLE void flip() const;

    /// @brief Pause training
    Q_INVOKABLE void pause() const;

    /// @brief Resume training
    Q_INVOKABLE void resume() const;

    /// @brief Reload current card
    Q_INVOKABLE void reloadCurrentCard();

  signals:
    void time_limit_changed();
    void time_remaining_changed();
    void front_changed();
    void back_changed();
    void flipped_changed();
    void card_changed();

  private:
    core::IStudyService& m_study_service;

    float time_remaining() const;
    int   time_limit() const;

    QString front() const;
    QString back() const;

    bool flipped() const;

    Card card() const;
};

} // namespace ui