// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "NotificationServiceStub.hpp" // for NotificationServiceStub header

namespace platform::internals {

void NotificationServiceStub::schedule_notification(const QString& text, const QDateTime& when) const {}

void NotificationServiceStub::show_notification(const QString& text) const {}

void NotificationServiceStub::clear_all_scheduled_notifications() const {}

} // namespace platform::internals
