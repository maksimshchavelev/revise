// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include "DeckMediaStorage.hpp" // for header
#include <QDir>                 // for QDir
#include <QStandardPaths>       // for QStandardPaths

namespace io {

DeckMediaStorage::DeckMediaStorage() :
    m_dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/media") {
    // Create images directory
    QDir().mkdir(m_dir);
}


std::expected<QString, QString> DeckMediaStorage::save_image(int               deck_id,
                                                             const QString&    image_name,
                                                             const QByteArray& binary_data) {
    const QString path = QString("%1/%2/%3").arg(m_dir).arg(deck_id).arg(image_name);
    QFile         file(path);

    if (QFile::exists(path)) {
        QFile::remove(path);
    }

    QDir().mkdir(QString("%1/%2").arg(m_dir).arg(deck_id));

    if (file.open(QIODevice::WriteOnly)) {
        file.write(binary_data);
        file.close();
        return path;
    } else {
        return std::unexpected(QString("Failed to open file '%1' for writing").arg(path));
    }
}


std::expected<void, QString> DeckMediaStorage::remove_image(const QString& absolute_path) {
    if (!QFile(absolute_path).exists()) {
        return std::unexpected(
            QString("Failed to remove deck media file: file '%1' does not exist").arg(absolute_path));
    }

    if (!QFile(absolute_path).remove()) {
        return std::unexpected(QString("Failed to remove deck media file: %1").arg(absolute_path));
    }

    return {};
}


std::expected<void, QString> DeckMediaStorage::remove_deck_media(int deck_id) {
    QDir dir(QString("%1/%2").arg(m_dir).arg(deck_id));

    if (dir.exists()) {
        if (!dir.removeRecursively()) {
            return std::unexpected(QString("Failed to remove deck media directory: %1").arg(dir.path()));
        }
    }

    return {};
}


QString DeckMediaStorage::to_export_path(const QString& absolute_path) const {
    return QFile(absolute_path).fileName();
}


QString DeckMediaStorage::deck_media_folder(int deck_id) const {
    return QString("%1/%2").arg(m_dir).arg(deck_id);
}


std::expected<void, QString> DeckMediaStorage::create_media_folder(int deck_id) {
    QDir dir(deck_media_folder(deck_id));

    if (dir.exists()) {
        return {};
    }

    if (!dir.mkdir(dir.absolutePath())) {
        return std::unexpected(QString("Failed to create deck media folder: %1").arg(dir.absolutePath()));
    }

    return {};
}


bool DeckMediaStorage::has_media_directory(int deck_id) const {
    return QDir(deck_media_folder(deck_id)).exists();
}

} // namespace io
