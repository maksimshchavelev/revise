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
    Q_PROPERTY(int maxInterval READ max_interval WRITE set_max_interval NOTIFY maxIntervalChanged FINAL)
    Q_PROPERTY(float learningRate READ learning_rate WRITE set_learning_rate NOTIFY learningRateChanged FINAL)
    Q_PROPERTY(float uiScale READ ui_scale WRITE set_ui_scale NOTIFY uiScaleChanged FINAL)
    Q_PROPERTY(float notifyAboutEarlyAccess READ notify_about_early_access WRITE set_notify_about_early_access NOTIFY
                   notifyAboutEarlyAccessChanged FINAL)


    explicit Settings(core::ISettings& settings, QObject* parent = nullptr);

  signals:
    void maxIntervalChanged();
    void learningRateChanged();
    void uiScaleChanged();
    void notifyAboutEarlyAccessChanged();

  private:
    /// @brief Get max card review interval
    [[nodiscard]] int max_interval() const;

    /// @brief Set max card review interval
    void set_max_interval(int interval);

    /// @brief Get learning rate
    [[nodiscard]] float learning_rate() const;

    /// @brief Set learning rate
    void set_learning_rate(float rate);

    /// @brief Get UI scale
    [[nodiscard]] float ui_scale() const;

    /// @brief Set UI scale
    void set_ui_scale(float scale);

    /// @brief Is notification about early access enabled?
    [[nodiscard]] bool notify_about_early_access() const;

    /// @brief Enable or disable early access notification
    void set_notify_about_early_access(bool enabled);

    core::ISettings& m_settings;
};

} // namespace ui
