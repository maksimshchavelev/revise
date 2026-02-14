// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include "Launcher/Launcher.hpp" // for Launcher

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    revise::Launcher launcher(app);
    return launcher.run();
}
