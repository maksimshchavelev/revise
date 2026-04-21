// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                     // for QObject
#include <core/settings/ISettings.hpp> // for core::ISettings

namespace ui {

class Settings final : public QObject {
    Q_OBJECT

  public:
    Q_PROPERTY(int maxInterval READ max_interval WRITE set_max_interval NOTIFY maxIntervalChanged FINAL)
    Q_PROPERTY(float learningRate READ learning_rate WRITE set_learning_rate NOTIFY learningRateChanged FINAL)
    Q_PROPERTY(QString applicationVersion READ application_version NOTIFY applicationVersionChanged FINAL)

    Settings(core::ISettings& settings, QObject* parent = nullptr);

  signals:
    void maxIntervalChanged();
    void learningRateChanged();
    void applicationVersionChanged();

  private:
    int  max_interval() const;
    void set_max_interval(int interval);

    float learning_rate() const;
    void  set_learning_rate(float rate);

    QString application_version() const;

    core::ISettings& m_settings;
};

} // namespace ui