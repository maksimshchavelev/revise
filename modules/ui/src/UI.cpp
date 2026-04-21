// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/UI.hpp"           // for UI header
#include "CardEditSession.hpp" // for CardEditSession
#include "CardsModel.hpp"      // for CardsModel wrapper
#include "DeckService.hpp"     // for DeckService wrapper
#include "DecksModel.hpp"      // for DecksModel wrapper
#include "Settings.hpp"        // for Settings
#include "StreakService.hpp"   // for StreakService wrapper
#include "StudyService.hpp"    // for StudyService
#include "ToastService.hpp"    // for ToastService
#include <QQmlContext>         // for QQmlContext

namespace ui {


void UI::init_engine(QGuiApplication& app) {
    m_engine.addImportPath("qrc:/");

    QObject::connect(
        &m_engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    m_engine.loadFromModule("Revise", "App");
}


void UI::enable_debug_bounds(bool enable) {
    m_engine.rootContext()->setContextProperty("uiShowBounds", enable);
}


void UI::bind_streak_service(core::IStreakService& streak_service) {
    auto* service = new StreakService(streak_service, &m_engine);
    m_engine.rootContext()->setContextProperty("streakService", service);
}


void UI::bind_deck_service(core::IDeckService& deck_service) {
    auto* service = new DeckService(deck_service, &m_engine);
    m_engine.rootContext()->setContextProperty("deckService", service);
}


void UI::bind_decks_model(core::IDeckService& deck_service, core::IStudyService& study_service) {
    auto* model = new DecksModel(deck_service, study_service, &m_engine);
    m_engine.rootContext()->setContextProperty("decksModel", model);
}


void UI::bind_cards_model(core::IDeckService& deck_service) {
    auto* model = new CardsModel(deck_service, &m_engine);
    m_engine.rootContext()->setContextProperty("cardsModel", model);
}


void UI::bind_study_service(core::IStudyService& study_service) {
    auto* service = new StudyService(study_service, &m_engine);
    m_engine.rootContext()->setContextProperty("studyService", service);
}


void UI::bind_toast_service(core::IToastService& toast_service) {
    auto* service = new ToastService(toast_service, &m_engine);
    m_engine.rootContext()->setContextProperty("toastService", service);
    qRegisterMetaType<ToastRequest>("ToastRequest");
}


void UI::bind_card_edit_session(core::ICardEditSession& session) {
    auto* service = new CardEditSession(session, &m_engine);
    m_engine.rootContext()->setContextProperty("cardEditSession", service);
}


void UI::bind_settings(core::ISettings& settings) {
    auto* s = new Settings(settings, &m_engine);
    m_engine.rootContext()->setContextProperty(QStringLiteral("settings"), s);
}


void UI::bind_loading_screen(LoadingScreen& screen) {
    m_engine.rootContext()->setContextProperty("loadingScreen", &screen);
}


void UI::bind_router(Router& router) {
    m_engine.rootContext()->setContextProperty("router", &router);
}


QQmlComponent* UI::create_page(QUrl source) {
    QQmlComponent* component{nullptr};

    QMetaObject::invokeMethod(
        &m_engine,
        [&]() {
            component = new QQmlComponent(&m_engine, std::move(source), &m_engine);

            if (component->isError()) {
                for (const auto& error : component->errors()) {
                    qWarning() << "ui::UI::create_page(): Failed to create component from file" << source
                               << "cause:" << error;
                }
            }
        },
        Qt::BlockingQueuedConnection);

    return component;
}

} // namespace ui