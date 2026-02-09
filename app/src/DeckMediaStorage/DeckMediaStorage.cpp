// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <DeckMediaStorage/DeckMediaStorage.hpp> // for DeckMediaStorage
#include <QDir>                                  // for QDir
#include <QStandardPaths>                        // for QStandardPaths

namespace revise {

// Public method
DeckMediaStorage::DeckMediaStorage() :
    m_dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/media") {
    // Create images directory
    QDir().mkdir(m_dir);
}

// Public method
std::expected<QString, QString> DeckMediaStorage::save_image(int               deckId,
                                                             const QString&    image_name,
                                                             const QByteArray& binary_data) const {
    const QString path = QString("%1/%2/%3").arg(m_dir).arg(deckId).arg(image_name);
    QFile         file(path);

    if (QFile::exists(path)) {
        QFile::remove(path);
    }

    QDir().mkdir(QString("%1/%2").arg(m_dir).arg(deckId));

    if (file.open(QIODevice::WriteOnly)) {
        file.write(binary_data);
        file.close();
        return path;
    } else {
        return std::unexpected(QString("Failed to open file '%1' for writing").arg(path));
    }
}

// Public method
std::expected<void, QString> DeckMediaStorage::remove_media(int deckId) const {
    QDir dir(QString("%1/%2").arg(m_dir).arg(deckId));

    if (dir.exists()) {
        if (!dir.removeRecursively()) {
            return std::unexpected(QString("Failed to remove deck media directory: %1").arg(dir.path()));
        }
    }

    return {};
}

// Public method
QString DeckMediaStorage::get_deck_media_folder(int deckId) const {
    const QDir dir = QString("%1/%2").arg(m_dir).arg(deckId);

    if (dir.exists()) {
        return dir.path();
    }

    return QString();
}

} // namespace revise
