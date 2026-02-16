// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "ui/UI.hpp"         // for UI header
#include "CardsModel.hpp"    // for CardsModel wrapper
#include "DeckService.hpp"   // for DeckService wrapper
#include "DecksModel.hpp"    // for DecksModel wrapper
#include "PopupService.hpp"  // for PopupService wrapper
#include "StreakService.hpp" // for StreakService wrapper
#include <QQmlContext>       // for QQmlContext

namespace ui {

void UI::init_qml() {
    // engine.rootContext()->setContextProperty("popupService", dynamic_cast<PopupService*>(&popup_service));

    qRegisterMetaType<PopupRequest>("PopupRequest");
    qRegisterMetaType<PopupResponse>("PopupResponse");
    qRegisterMetaType<PopupAction>("PopupAction");
    qRegisterMetaType<PopupButton>("PopupButton");
}


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


void UI::bind_streak_service(core::IStreakService& streak_service) {
    auto* service = new StreakService(streak_service, &m_engine);
    m_engine.rootContext()->setContextProperty("streakService", service);
}


void UI::bind_deck_service(core::IDeckService& deck_service) {
    auto* service = new DeckService(deck_service, &m_engine);
    m_engine.rootContext()->setContextProperty("deckService", service);
}


void UI::bind_decks_model(core::IDeckService& deck_service) {
    auto* model = new DecksModel(deck_service, &m_engine);
    m_engine.rootContext()->setContextProperty("decksModel", model);
}


void UI::bind_cards_model(core::IDeckService& deck_service) {
    auto* model = new CardsModel(deck_service, &m_engine);
    m_engine.rootContext()->setContextProperty("cardsModel", model);
}

} // namespace ui