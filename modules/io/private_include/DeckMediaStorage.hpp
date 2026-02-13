// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <core/IDeckMediaStorage.hpp> // for core::IDeckMediaStorage

namespace io {

/**
 * @brief Implementation of a media file storage system for decks
 */
class DeckMediaStorage final : public core::IDeckMediaStorage {
  public:
    DeckMediaStorage();

    /// @copydoc core::IDeckMediaStorage::save_image
    std::expected<QString, QString> save_image(int               deck_id,
                                               const QString&    image_name,
                                               const QByteArray& binary_data) override;

    /// @copydoc core::IDeckMediaStorage::remove_image
    std::expected<void, QString> remove_image(const QString& absolute_path) override;

    /// @copydoc core::IDeckMediaStorage::remove_deck_media
    std::expected<void, QString> remove_deck_media(int deck_id) override;

    /// @copydoc core::IDeckMediaStorage::to_export_path
    QString to_export_path(const QString& absolute_path) const override;

    /// @copydoc core::IDeckMediaStorage::deck_media_storage
    QString deck_media_folder(int deck_id) const override;

  private:
    const QString m_dir; ///< Directory with media
};

} // namespace io
