// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/settings/ISettings.hpp>        // for core::ISettings

namespace ui {

class Settings final : public QObject {
    Q_OBJECT

    QML_NAMED_ELEMENT(SettingsService)
    QML_SINGLETON
    QML_UNCREATABLE("SettingsService is provided by C++")

  public:
    // ------------- Algorithm properties -------------
    Q_PROPERTY(int maxInterval READ max_interval WRITE set_max_interval NOTIFY maxIntervalChanged FINAL)
    Q_PROPERTY(float learningRate READ learning_rate WRITE set_learning_rate NOTIFY learningRateChanged FINAL)

    // ------------- Main properties -------------
    Q_PROPERTY(float uiScale READ ui_scale WRITE set_ui_scale NOTIFY uiScaleChanged FINAL)
    Q_PROPERTY(bool notifyAboutEarlyAccess READ notify_about_early_access WRITE set_notify_about_early_access NOTIFY
                   notifyAboutEarlyAccessChanged FINAL)
    Q_PROPERTY(bool notifyAboutTooOldAndroid READ notify_about_too_old_android WRITE set_notify_about_too_old_android NOTIFY
                   notifyAboutTooOldAndroidChanged FINAL)
    Q_PROPERTY(bool useMobileUi READ use_mobile_ui WRITE set_use_mobile_ui NOTIFY useMobileUiChanged FINAL)

    // ------------- Dialog properties -------------
    Q_PROPERTY(bool askRemoveOneCard READ ask_remove_one_card WRITE set_ask_remove_one_card NOTIFY askRemoveOneCardChanged FINAL)
    Q_PROPERTY(
        bool askRemoveManyCards READ ask_remove_many_cards WRITE set_ask_remove_many_cards NOTIFY askRemoveManyCardsChanged FINAL)
    Q_PROPERTY(
        bool askRelearnOneCard READ ask_relearn_one_card WRITE set_ask_relearn_one_card NOTIFY askRelearnOneCardChanged FINAL)
    Q_PROPERTY(bool askRelearnManyCards READ ask_relearn_many_cards WRITE set_ask_relearn_many_cards NOTIFY
                   askRelearnManyCardsChanged FINAL)
    Q_PROPERTY(bool askRemoveDeck READ ask_remove_deck WRITE set_ask_remove_deck NOTIFY askRemoveDeckChanged FINAL)
    Q_PROPERTY(bool askSetDifficultyForManyCards READ ask_set_difficulty_for_many_cards WRITE
                   set_ask_set_difficulty_for_many_cards NOTIFY askSetDifficultyForManyCardsChanged FINAL)
    Q_PROPERTY(bool askSetStateForManyCards READ ask_set_state_for_many_cards WRITE set_ask_set_state_for_many_cards NOTIFY
                   askSetStateForManyCardsChanged FINAL)
    Q_PROPERTY(bool askAbortLearning READ ask_abort_learning WRITE set_ask_abort_learning NOTIFY
                   askAbortLearningChanged FINAL)


    explicit Settings(core::ISettings& settings, QObject* parent = nullptr);

  signals:
    // ------------- Algorithm settings signals -------------
    void maxIntervalChanged();
    void learningRateChanged();

    // ------------- Main settings signals -------------
    void uiScaleChanged();
    void notifyAboutEarlyAccessChanged();
    void notifyAboutTooOldAndroidChanged();
    void useMobileUiChanged();

    // ------------- Dialog settings signals -------------
    void askRemoveOneCardChanged();
    void askRemoveManyCardsChanged();
    void askRelearnOneCardChanged();
    void askRelearnManyCardsChanged();
    void askRemoveDeckChanged();
    void askSetDifficultyForManyCardsChanged();
    void askSetStateForManyCardsChanged();
    void askAbortLearningChanged();

  private:
    // ----------------------- Study algorithm settings -----------------------
    /// @brief Get max card review interval
    [[nodiscard]] int max_interval() const;

    /// @brief Set max card review interval
    void set_max_interval(int interval);

    /// @brief Get learning rate
    [[nodiscard]] float learning_rate() const;

    /// @brief Set learning rate
    void set_learning_rate(float rate);

    // ----------------------- Main settings -----------------------
    /// @brief Get UI scale
    [[nodiscard]] float ui_scale() const;

    /// @brief Set UI scale
    void set_ui_scale(float scale);

    /// @brief Is notification about early access enabled?
    [[nodiscard]] bool notify_about_early_access() const;

    /// @brief Enable or disable early access notification
    void set_notify_about_early_access(bool enabled);

    /// @brief Show notification about too old mobile
    [[nodiscard]] bool notify_about_too_old_android() const;

    /// @brief Enable or disable notification about too old mobile
    void set_notify_about_too_old_android(bool enabled);

    /// @brief Use mobile or desktop UI?
    [[nodiscard]] bool use_mobile_ui() const;

    /// @brief Set usage of mobile UI
    void set_use_mobile_ui(bool enabled);

    // ----------------------- Dialog settings -----------------------
    /// @brief Ask when user wants to remove one card?
    [[nodiscard]] bool ask_remove_one_card() const;

    /// @brief Specify whether to prompt the user when they delete a single card
    void set_ask_remove_one_card(bool enabled);

    /// @brief Ask when user wants to remove many cards?
    [[nodiscard]] bool ask_remove_many_cards() const;

    /// @brief Specify whether to prompt the user when they delete many cards
    void set_ask_remove_many_cards(bool enabled);

    /// @brief Ask when user wants to relearn one card?
    [[nodiscard]] bool ask_relearn_one_card() const;

    /// @brief Specify whether to prompt the user when they want to relearn one card
    void set_ask_relearn_one_card(bool enabled);

    /// @brief Ask when user wants to relearn many cards?
    [[nodiscard]] bool ask_relearn_many_cards() const;

    /// @brief Specify whether to prompt the user when they want to relearn many cards
    void set_ask_relearn_many_cards(bool enabled);

    /// @brief Ask when user wants to remove one deck?
    [[nodiscard]] bool ask_remove_deck() const;

    /// @brief Specify whether to prompt the user when they want to remove one deck
    void set_ask_remove_deck(bool enabled);

    /// @brief Ask when user wants to set difficulty for many cards?
    [[nodiscard]] bool ask_set_difficulty_for_many_cards() const;

    /// @brief Specify whether to prompt the user when they want to set difficulty for many cards
    void set_ask_set_difficulty_for_many_cards(bool enabled);

    /// @brief Ask when user wants to set state for many cards?
    [[nodiscard]] bool ask_set_state_for_many_cards() const;

    /// @brief Specify whether to prompt the user when they want to set state for many cards
    void set_ask_set_state_for_many_cards(bool enabled);

    /// @brief Ask when user wants to abort learning?
    [[nodiscard]] bool ask_abort_learning() const;

    /// @brief Specify whether to prompt the user when they want to abort learning
    void set_ask_abort_learning(bool enabled);

    core::ISettings& m_settings;
};

} // namespace ui
