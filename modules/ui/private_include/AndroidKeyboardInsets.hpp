// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>            // for QObject
#include <QtGlobal>           // for qreal
#include <qqmlintegration.h>  // for QML_ELEMENT, QML_SINGLETON, QML_UNCREATABLE

namespace ui {

/// \class AndroidKeyboardInsets
/// \brief Provides Android soft keyboard geometry to QML.
///
/// AndroidKeyboardInsets retrieves input method editor (IME) insets from
/// the Android WindowInsets API and exposes keyboard geometry as Qt properties.
///
/// The native Android listener reports the bottom IME inset in physical pixels.
/// The class converts this value into Qt logical pixels using the device pixel ratio
/// of the primary screen.
///
/// Keyboard visibility and geometry are updated during keyboard appearance
/// and disappearance animations.
///
/// This class is provided as a C++-owned QML singleton and cannot be instantiated
/// directly from QML.
///
/// \qmltype AndroidKeyboardInsets
/// \inqmlmodule Revise
/// \since 1.0
class AndroidKeyboardInsets final : public QObject
{
    Q_OBJECT

    QML_ELEMENT
    QML_SINGLETON
    QML_UNCREATABLE("AndroidKeyboardInsets is provided by C++")

    /// \qmlproperty real AndroidKeyboardInsets::imeHeight
    /// \brief Current keyboard height in Qt logical pixels.
    ///
    /// Represents the bottom inset occupied by the on-screen keyboard.
    /// The value is updated during keyboard appearance and disappearance
    /// animations.
    ///
    /// Returns zero when the keyboard does not occupy the bottom area
    /// of the application window, for example when using a floating keyboard.
    Q_PROPERTY(qreal imeHeight
               READ imeHeight
               NOTIFY imeInsetsChanged
               FINAL)

    /// \qmlproperty bool AndroidKeyboardInsets::imeVisible
    /// \brief Indicates whether the on-screen keyboard is visible.
    ///
    /// The value is obtained from Android WindowInsets::isVisible()
    /// for the IME inset type.
    Q_PROPERTY(bool imeVisible
               READ imeVisible
               NOTIFY imeInsetsChanged
               FINAL)

public:
    /// Constructs the Android keyboard insets provider.
    ///
    /// \param parent QObject parent.
    explicit AndroidKeyboardInsets(QObject *parent = nullptr);

    /// Installs the native Android keyboard insets listener.
    ///
    /// The listener is installed on the Android UI thread and observes
    /// IME insets applied to the Activity content view.
    ///
    /// On non-Android platforms, this function does nothing.
    ///
    /// The listener reports keyboard geometry through the JNI callback,
    /// which subsequently updates this object's properties.
    void install();

    /// Returns the current keyboard height in Qt logical pixels.
    ///
    /// \return Current IME bottom inset converted from physical pixels
    ///         into Qt logical pixels.
    [[nodiscard]] qreal imeHeight() const noexcept;

    /// Returns whether the Android on-screen keyboard is visible.
    ///
    /// \return \c true if the IME is visible; otherwise, \c false.
    [[nodiscard]] bool imeVisible() const noexcept;

    /// Dispatches keyboard geometry received from the Android JNI callback.
    ///
    /// This function can be called from the Android UI thread.
    /// The property update is queued and performed in the thread
    /// to which this QObject belongs.
    ///
    /// \param bottomPx Current IME bottom inset in physical pixels.
    /// \param visible Whether the IME is visible.
    void dispatchImeInsets(int bottomPx, bool visible);

signals:
    /// Emitted whenever keyboard visibility or geometry changes.
    ///
    /// This signal notifies QML bindings and other connected objects
    /// that the values of \c imeHeight or \c imeVisible may have changed.
    void imeInsetsChanged();

private:
    /// Updates the keyboard visibility and geometry properties.
    ///
    /// This function must execute in the thread to which this QObject belongs.
    ///
    /// Converts the physical pixel inset into Qt logical pixels using
    /// the device pixel ratio of the primary screen.
    ///
    /// Emits \c imeInsetsChanged() only when at least one property changes.
    ///
    /// \param bottomPx Current IME bottom inset in physical pixels.
    /// \param visible Whether the IME is visible.
    void updateInsets(int bottomPx, bool visible);

    qreal m_imeHeight{0.0}; ///< Current keyboard height in Qt logical pixels.
    bool  m_imeVisible{false}; ///< Whether the Android keyboard is currently visible.
};

} // namespace ui
