// Copyright 2026 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

#include <QObject>                            // for QObject
#include <QtQmlIntegration/qqmlintegration.h> // for QML_SINGLETON, QML_NAMED_ELEMENT
#include <core/BuildInfo.hpp>                 // for BuildInfo

namespace ui {

/// @brief Provides access to information about the application build, such as the version or compiler flags
class BuildInfo : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(BuildInfo)
    QML_SINGLETON

    Q_PROPERTY(QString version MEMBER version CONSTANT)
    Q_PROPERTY(QString compilerId MEMBER compilerId CONSTANT)
    Q_PROPERTY(QString compilerVersion MEMBER compilerVersion CONSTANT)
    Q_PROPERTY(QString targetArch MEMBER targetArch CONSTANT)
    Q_PROPERTY(QString hostArch MEMBER hostArch CONSTANT)
    Q_PROPERTY(QString buildType MEMBER buildType CONSTANT)
    Q_PROPERTY(QString buildFlags MEMBER buildFlags CONSTANT)
    Q_PROPERTY(QString buildHostSystem MEMBER buildHostSystem CONSTANT)
    Q_PROPERTY(QString buildGitCommitHash MEMBER buildGitCommitHash CONSTANT)
    Q_PROPERTY(QString buildQtVersion MEMBER buildQtVersion CONSTANT)
    Q_PROPERTY(bool isBetaBuild MEMBER isBetaBuild CONSTANT)

  public:
    explicit BuildInfo(QObject* parent = nullptr) : QObject(parent) {}

    QString version =
        QString("%1.%2.%3").arg(core::REVISE_VERSION_MAJOR).arg(core::REVISE_VERSION_MINOR).arg(core::REVISE_VERSION_PATCH);

    QString compilerId = core::REVISE_BUILD_COMPILER_ID;
    QString compilerVersion = core::REVISE_BUILD_COMPILER_VERSION;
    QString targetArch = core::REVISE_BUILD_TARGET_ARCH;
    QString hostArch = core::REVISE_BUILD_HOST_ARCH;
    QString buildType = core::REVISE_BUILD_TYPE;
    QString buildFlags = core::REVISE_BUILD_FLAGS;
    QString buildHostSystem = core::REVISE_BUILD_HOST_SYSTEM;
    QString buildGitCommitHash = core::REVISE_BUILD_GIT_COMMIT_HASH;
    QString buildQtVersion = core::REVISE_QT_VERSION;
    bool    isBetaBuild = REVISE_BETA_BUILD;
};

} // namespace ui
