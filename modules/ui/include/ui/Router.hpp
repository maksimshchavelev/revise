// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QHash>         // for QHash
#include <QObject>       // for QObject
#include <QQmlComponent> // for QQmlComponent
#include <QVector>       // for QVector

namespace ui {

/**
 * @brief Represents a navigable page in the application.
 *
 * The Page struct encapsulates the identity of a page and any parameters
 * that should be passed to it when navigating. It is used by the Router
 * to tell QML which page to load and with what data.
 *
 * @note Visible from QML
 */
struct Page {
    Q_GADGET
    QML_VALUE_TYPE(page)

    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QVariantMap params MEMBER params)
    Q_PROPERTY(OpenMode mode MEMBER mode)

  public:
    /// @brief How to open a page?
    enum class OpenMode {
        Page,  ///< Open the page as a new tab
        Window ///< Open the page in a separate window (*if supported by the platform*)
    };

    Q_ENUM(OpenMode)

    QString     name;   ///< Name of page
    QVariantMap params; ///< Parameters of page
    OpenMode    mode;   ///< Open mode
};

/**
 * @brief Router for application navigation using preloaded pages.
 *
 * This class manages navigation between pages by storing already loaded
 * QQmlComponent instances associated with string identifiers.
 *
 * QML can display the current page by binding a Loader's sourceComponent
 * to currentPageComponent. All navigation is performed through the C++ Router;
 * QML only reacts to the currently active page.
 */
class Router final : public QObject {
    Q_OBJECT

    Q_PROPERTY(Page currentPage READ current_page NOTIFY pageChanged FINAL)
    Q_PROPERTY(QQmlComponent* currentPageComponent READ current_page_component NOTIFY pageChanged FINAL)

  public:
    /**
     * @brief Register a page with a unique name.
     *
     * This method allows preloading pages into the router. Once a page is
     * pushed, it can be navigated to by its name.
     *
     * @param name Unique identifier for the page.
     * @param page Pointer to a QQmlComponent representing the page.
     */
    void push_page(QString name, QQmlComponent* page);

    /**
     * @brief Navigate to a previously pushed page.
     *
     * Changes the current page to the one registered with the given name.
     *
     * @param page Page. See `ui::Page`
     *
     * @note The page opening mode can be changed (for example, if `Window` is not supported by the platform, the mode
     * will be changed to `Page`)
     */
    void navigate(Page page);

    /**
     * @brief Navigate to a previously pushed page.
     *
     * Changes the current page to the one registered with the given name.
     *
     * @param page_name Name of page
     * @param params Params of page
     * @param mode How to open a page? See `ui::Page::OpenMode`
     *
     * @note Can be called from QML
     *
     * @note The page opening mode can be changed (for example, if `Window` is not supported by the platform, the mode
     * will be changed to `Page`)
     */
    Q_INVOKABLE void navigate(QString page_name, QVariantMap params = {}, Page::OpenMode mode = Page::OpenMode::Page);

    /**
     * @brief Navigate back to the previous page.
     *
     * If a history stack is maintained, this method restores the previous page.
     * If no previous page exists, behavior is implementation-defined (e.g., stay
     * on current page).
     *
     * @note Can be called from QML
     */
    Q_INVOKABLE void back();

    /**
     * @brief Get the currently active page component.
     *
     * QML can bind a Loader's sourceComponent to this value to display the
     * current page.
     *
     * @return QQmlComponent* Pointer to the currently active page component.
     */
    QQmlComponent* current_page_component();

    /**
     * @brief  Get the currently active page
     *
     * @return `Page`
     */
    Page current_page() const;

  signals:
    void pageChanged();

  private:
    QVector<Page>                                                       m_history; ///< Navigation history
    QHash<QString /* page name */, QQmlComponent* /* page component */> m_pages;   ///< Pages
};

} // namespace ui
