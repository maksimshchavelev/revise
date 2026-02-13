// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QByteArray> // for QByteArray
#include <QString>    // for QString
#include <expected>   // for std::expected

namespace core {

/**
 * @brief Media storage interface for deck images.
 *
 * Provides abstraction for saving, loading and removing deck-related media.
 */
class IDeckMediaStorage {
  public:
    virtual ~IDeckMediaStorage() = default;

    /**
     * @brief Saves an image of a specific deck
     * @param deck_id ID of the deck to which the image belongs
     * @param image_name Image name
     * @param binary_data Binary image data
     * @return `std::expected<QString, QString>` with an **absolute** path to the
     * image if successful, otherwise an error description
     */
    virtual std::expected<QString, QString> save_image(int               deck_id,
                                                       const QString&    image_name,
                                                       const QByteArray& binary_data) = 0;

    /**
     * @brief Remove image by absolute path.
     * @param absolute_path Absolute path to image
     * Used when user deletes image from card.
     * @return `std::expected<void, QString>` with an **absolute** `void` if success, otherwise an error description
     */
    virtual std::expected<void, QString> remove_image(const QString& absolute_path) = 0;

    /**
     * @brief Remove all media for deck.
     * Used on deck deletion.
     * @param deck_id Deck ID
     * @return `std::expected<void, QString>` with an **absolute** `void` if success, otherwise an error description
     */
    virtual std::expected<void, QString> remove_deck_media(int deck_id) = 0;

    /**
     * @brief Convert absolute path to export-relative path.
     * @param absolute_path Absolute path to image
     * @return The path that is ready for deck export (for example, just image name)
     */
    virtual QString to_export_path(const QString& absolute_path) const = 0;

    /**
     * @brief Get the **absolute** path to the directory where the deck's media files are stored
     * @param deck_id Deck ID
     * @return The path
     */
    virtual QString deck_media_folder(int deck_id) const = 0;
};

} // namespace core
