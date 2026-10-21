// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE
#include <core/settings/ISettings.hpp>        // for core::ISettings

namespace ui {

class Settings final : public QObject {
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("Settings is provided by C++")

  public:
    Q_PROPERTY(int maxInterval READ max_interval WRITE set_max_interval NOTIFY maxIntervalChanged FINAL)
    Q_PROPERTY(float learningRate READ learning_rate WRITE set_learning_rate NOTIFY learningRateChanged FINAL)

    explicit Settings(core::ISettings& settings, QObject* parent = nullptr);

  signals:
    void maxIntervalChanged();
    void learningRateChanged();

  private:
    /// @brief Get max card review interval
    int max_interval() const;

    /// @brief Set max card review interval
    void set_max_interval(int interval) const;

    /// @brief Get learning rate
    float learning_rate() const;

    /// @brief Set learning rate
    void set_learning_rate(float rate) const;

    core::ISettings& m_settings;
};

} // namespace ui