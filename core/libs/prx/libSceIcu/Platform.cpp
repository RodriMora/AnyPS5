// Derived from icu4c/source/common/putil.cpp@049e0d6a420629ac7db77256987d083a563287b5 (ICU 78.1): uprv_getUTCtime, uprv_getRawUTCtime, uprv_pathIsAbsolute and uprv_getDefaultLocaleID below.
// https://github.com/unicode-org/icu/blob/049e0d6a420629ac7db77256987d083a563287b5/icu4c/source/common/putil.cpp
//
// The upstream file's notice, verbatim:
//   © 2016 and later: Unicode, Inc. and others.
//   License & terms of use: http://www.unicode.org/copyright.html
//   Copyright (C) 2016, International Business Machines
//   Corporation and others.  All Rights Reserved.

#include "prx/libSceIcu/Platform.hpp"
#include "prx/libc/include/Api.hpp"
#include "prx/libc/include/General.hpp"
#include <chrono>
#include <cstring>
#include <mutex>
#include <string>
#include <utility>

namespace {

constexpr char FileSeparator = '/';
constexpr char AlternativeSeparator = '/';
constexpr int AllLocaleCategories = 0;

std::string CorrectedPosixLocale;
std::once_flag PosixLocaleOnce;

const char* PosixIDForDefaultLocale() {
    const char* locale = setlocale_nid_postfix(AllLocaleCategories, nullptr);
    if (!locale || std::strcmp(locale, "C") == 0 || std::strcmp(locale, "POSIX") == 0) {
        for (const char* name : {"LC_ALL", "LC_MESSAGES", "LANG"}) {
            const char* environment = getenv_nid_postfix(name);
            if (environment) return environment;
        }
        return nullptr;
    }
    return locale;
}

const char* DefaultLocaleID() {
    std::call_once(PosixLocaleOnce, [] {
        const char* posixID = PosixIDForDefaultLocale();
        const char* name = posixID ? posixID : "en_US_POSIX";
        std::string corrected(name);
        const auto dot = corrected.find('.');
        if (dot != std::string::npos) corrected.resize(dot);
        const auto at = corrected.find('@');
        if (at != std::string::npos) corrected.resize(at);
        if (corrected == "C" || corrected == "POSIX") corrected = "en_US_POSIX";
        const std::string raw(name);
        const auto rawAt = raw.rfind('@');
        if (rawAt != std::string::npos) {
            std::string variant = raw.substr(rawAt + 1);
            if (variant == "nynorsk") variant = "NY";
            corrected += corrected.find('_') == std::string::npos ? "__" : "_";
            const auto variantDot = variant.find('.');
            if (variantDot != std::string::npos) variant.resize(variantDot);
            corrected += variant;
        }
        CorrectedPosixLocale = std::move(corrected);
    });
    return CorrectedPosixLocale.c_str();
}

}

extern "C" {

double APS5_VABI uprv_getUTCtime_nid_postfix() {
    return uprv_getRawUTCtime_nid_postfix();
}

double APS5_VABI uprv_getRawUTCtime_nid_postfix() {
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
}

std::int8_t APS5_VABI uprv_pathIsAbsolute_nid_postfix(const char* path) {
    if (!path || !*path) return 0;
    if (*path == FileSeparator || *path == AlternativeSeparator) return 1;
    return 0;
}

const char* APS5_VABI uprv_getDefaultLocaleID_nid_postfix() {
    return DefaultLocaleID();
}

}

namespace {

void ResetDefaultLocaleID() {
    CorrectedPosixLocale.clear();
    PosixLocaleOnce.~once_flag();
    new (&PosixLocaleOnce) std::once_flag;
}

}

extern "C" void IcuResetDefaultLocaleID_nid_no_patch() {
    ResetDefaultLocaleID();
}
