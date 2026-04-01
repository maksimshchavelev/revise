// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QString>                   // for QString
#include <core/ICardEditSession.hpp> // for core::ICardEditSession

namespace engine {

class CardEditSession : public core::ICardEditSession {
  public:
    /// @copydoc core::ICardEditSession::begin
    void begin(QString front, QString back) override;

    /// @copydoc core::ICardEditSession::end
    void end() override;

    /// @copydoc core::ICardEditSession::set_front
    void set_front(QString front) override;

    /// @copydoc core::ICardEditSession::front
    QString front() const override;

    /// @copydoc core::ICardEditSession::set_back
    void set_back(QString back) override;

    /// @copydoc core::ICardEditSession::back
    QString back() const override;

  private:
    QString m_front; ///< Card front
    QString m_back;  ///< Card back
};

} // namespace engine
