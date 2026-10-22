// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>  // for QString
#include <QVariant> // for QVariant
#include <QVector>  // for QVector
#include <expected> // for std::expected

namespace core {

/**
 * @brief Card view event. Used with some implementation of the `IEventRecorder` event recorder.
 */
struct CardReviewEvent {
    int event_id{0}; ///< Used only when selecting events via `core::IEventStorage`. Not used when saving an event.
    int card_id;     ///< Local Card ID
    int global_card_id{0};                     ///< Global card ID
    int deck_id;                               ///< Local deck ID
    int global_deck_id{0};                     ///< Global deck ID
    int current_streak;                        ///< Current user streak
    std::chrono::milliseconds review_duration; ///< How much time did the user spend on the answer?
};

/**
 * @brief A enumeration used to determine the type of event in the event system
 */
enum class EventType {
    CARD_REVIEW = 0 ///< Card viewing event
};

/**
 * @brief This list describes the synchronization status of an event in the event system.
 */
enum class EventSyncState {
    NOT_SYNCED = 0, ///< Not synchronized with anything
    LOCAL = 1,      ///< Synchronized only locally
    REMOTE = 2,     ///< Synchronized with remote server
    FULLY = 3       ///< Synchronized both locally and with a remote server
};

/**
 * @brief A class that manages the event store. This is an interface; implement the functionality yourself.
 */
class IEventStorage {
  public:
    virtual ~IEventStorage() = default;

    /**
     * @brief Records the event of viewing a card
     * @param event Card review event
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> record(const CardReviewEvent& event) = 0;

    /**
     * @brief Selects events that have a specific synchronization flag and a specific type
     * @param event_type Event type
     * @param sync_state Synchronization status
     * @limit Limit on the number of records retrieved
     * @return `std::expected<void, QString>` with  (`QVector` from `QVariant` (you need to extract the required event
     * structure from `QVariant` yourself, for example, `CardReviewEvent`) if success, otherwise error description
     */
    virtual std::expected<QVector<QVariant>, QString> fetch_synced(const EventType      event_type,
                                                                   const EventSyncState sync_state,
                                                                   int                  limit = 100) = 0;

    /**
     * @brief Set a synchronization flag for an event
     * @param ids Vector of event IDs that need to be updated
     * @param sync_state Synchronization status
     * @return `std::expected<void, QString>` with `void` if success, otherwise error description
     */
    virtual std::expected<void, QString> mark_synced_batch(QVector<int> ids, const EventSyncState sync_state) = 0;
};

} // namespace core
