// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/QmlIncubator.hpp" // for header
#include <QQuickItem>          // for QQuickItem

namespace ui {

void QmlIncubator::statusChanged(Status status) {
    if (status != Status::Ready) {
        return;
    }

    QObject*    page_object = object();
    QQuickItem* page_item = qobject_cast<QQuickItem*>(page_object);

    if (!page_item) {
        qWarning() << "QmlIncubator::statusChanged(): failed to get page item: got nullptr";
        return;
    }

    page_item->setVisible(false);
}

} // namespace ui
