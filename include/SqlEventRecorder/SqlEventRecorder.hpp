// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <Database/Database.hpp>             // for Database
#include <IEventRecorder/IEventRecorder.hpp> // for IEventRecorder

namespace revise {

/**
 * @brief Implementation of `IEventRecorder`, which saves events to a database
 */
class SqlEventRecorder : public IEventRecorder {
  public:
    SqlEventRecorder(Database& db);

    std::expected<void, QString> record(const CardReviewEvent& event) override;

  private:
    Database& m_db;
};

} // namespace revise
