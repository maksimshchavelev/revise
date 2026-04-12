// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#include <QtGlobal> // for Q_DECL_EXPORT

#pragma once

#ifdef _WIN32
    #ifdef UI_BUILD_SHARED
        #define UI_EXPORT Q_DECL_EXPORT
    #else
        #define UI_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define UI_EXPORT
#endif
