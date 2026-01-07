// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>
// A class that regulates the learning process. It determines which cards will be included in today's selection and
// manages the training process for the selected deck.

#pragma once

#include <Database/Database.hpp>               // for Database
#include <IAlgorithm/IAlgorithm.hpp>           // for IAlgorithm
#include <IDeckRepository/IDeckRepository.hpp> // for IDeckRepository
#include <QGuiApplication>                     // for QGuiApplication
#include <QObject>                             // for QObject
#include <QQueue>                              // for QQueue
#include <QTimer>                              // for QTimer

namespace revise {

/**
 * @brief Service that manages study sessions for a deck.
 */
class StudyService : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(int timeLimit READ time_limit NOTIFY time_limit_changed)
    Q_PROPERTY(float timeRemaining READ time_remaining NOTIFY time_remaining_changed)
    Q_PROPERTY(QString cardText READ card_text NOTIFY card_text_changed)
    Q_PROPERTY(bool flipped READ flipped NOTIFY flipped_changed)

    /**
     * @brief Study statistics for a deck (used by UI models).
     */
    struct StudyInfo {
        int new_cards{0};
        int consolidate_cards{0};
        int incorrect_cards{0};
        int time_limit{0};
    };

    /**
     * @brief Construct StudyService.
     * @param repo Reference to repository used for loading and persisting cards.
     * @param algo Reference to scheduling algorithm (must outlive this service).
     * @param parent QObject parent.
     */
    explicit StudyService(IDeckRepository& repo, IAlgorithm& algo, QObject* parent = nullptr);

    ~StudyService() override = default;

    QString card_text() const noexcept;
    int     time_limit() const noexcept;
    float   time_remaining() const noexcept;
    bool    flipped() const noexcept;

    /**
     * @brief Prepare and start training for the specified deck.
     * This method loads cards from repository, applies limits and fills the internal queue.
     * @param deck_id Deck id
     * @note In case of an error, `ErrorReporter` is called.
     */
    Q_INVOKABLE void start_training(int deck_id);

    /**
     * @brief Process next card with user's selected difficulty.
     * @param current_difficulty float in [0..5], where 5.0 means "I failed".
     * @note In case of an error, `ErrorReporter` is called.
     */
    Q_INVOKABLE void next_card(float current_difficulty);

    /**
     * @brief Flip the current card (show back) and reset timer.
     */
    Q_INVOKABLE void flip() noexcept;

    /**
     * @brief Abort training
     * @note Observe signal `aborted()`
     */
    Q_INVOKABLE void abort();

    /**
     * @brief Pause timer
     */
    Q_INVOKABLE void pause();

    /**
     * @brief Resume timer
     */
    Q_INVOKABLE void resume();

    /**
     * @brief Get study info for a deck (counts of new/consolidate/incorrect and time limit).
     * @param deck_id Deck id
     * @return StudyInfo (always returns a value; on repo error returns empty stats and logs warning).
     */
    StudyInfo get_deck_study_info(int deck_id);

    /**
     * @brief Whether a training session is active.
     */
    Q_INVOKABLE bool is_session_active() const noexcept {
        return !m_cards.isEmpty();
    }

  signals:
    void card_text_changed();
    void time_limit_changed();
    void time_remaining_changed();
    void training_finished();
    void flipped_changed();
    void aborted();

    /**
     * @brief Emitted when a non-fatal error occurs (e.g. DB update failure).
     */
    void error_occurred(const QString& error);

  private slots:
    void on_timer_timeout();

  private:
    IDeckRepository& m_repo;    ///< repository for DB access
    IAlgorithm&      m_algo;    ///< algorithm used to compute next review
    QQueue<Card>     m_cards;   ///< active queue of cards
    QList<Card>      m_trained; ///< trained cards to persist at end
    QTimer           m_timer;   ///< timer for smooth time decrease
    int              m_time_limit{0};
    float            m_time_remaining{0.0f};
    bool             m_flipped{false};
    bool             m_timer_paused{false};
    int              m_current_deck_id{0};

    // helpers
    void enqueue_all(const QList<Card>& list);
};

} // namespace revise
