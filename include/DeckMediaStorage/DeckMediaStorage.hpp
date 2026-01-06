// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <expected>   // for std::expected
#include <QString>    // for QString
#include <QByteArray> // for QByteArray

namespace revise {

/**
 * @brief The class manages media content belonging to the deck.
 */
class DeckMediaStorage {
public:
    DeckMediaStorage();

    /**
     * @brief Saves the transferred image
     * @param deckId ID of the deck to which the image belongs
     * @param image_name Image name
     * @param binary_data Binary image data
     * @return `std::expected<QString, QString>` with an **absolute** path to the
     * image if successful, otherwise an error description
     */
    std::expected<QString, QString> save_image(int deckId,
                                               const QString& image_name,
                                               const QByteArray& binary_data) const;

    /**
     * @brief Deletes all media data from the specified deck
     * @param deckId ID of the deck
     * @return `std::expected<void, QString>`
     */
    std::expected<void, QString> remove_media(int deckId) const;

private:
    const QString m_dir; ///< Directory with media
};

}
