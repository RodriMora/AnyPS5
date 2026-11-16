#include "prx/libSceIcu/libSceIcu.hpp"
#include "prx/libc/include/Api.hpp"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <source_location>

namespace {

void Require(bool condition, std::source_location location = std::source_location::current()) {
    if (condition) return;
    std::fprintf(stderr, "ICU platform check failed at line %u\n", location.line());
    std::abort();
}

void ExpectLocale(const char* expected, std::source_location location = std::source_location::current()) {
    const char* actual = uprv_getDefaultLocaleID_nid_postfix();
    if (actual && std::strcmp(actual, expected) == 0) return;
    std::fprintf(stderr, "ICU platform check failed at line %u: expected %s\n", location.line(), expected);
    std::abort();
}

void Set(const char* name, const char* value) {
    Require(setenv_nid_postfix(name, value, 1) == 0);
}

void UnsetLocaleEnvironment() {
    Require(unsetenv_nid_postfix("LC_ALL") == 0);
    Require(unsetenv_nid_postfix("LC_MESSAGES") == 0);
    Require(unsetenv_nid_postfix("LANG") == 0);
}

void PathChecks() {
    Require(uprv_pathIsAbsolute_nid_postfix(nullptr) == 0);
    Require(uprv_pathIsAbsolute_nid_postfix("") == 0);
    Require(uprv_pathIsAbsolute_nid_postfix("/") == 1);
    Require(uprv_pathIsAbsolute_nid_postfix("//server/share") == 1);
    Require(uprv_pathIsAbsolute_nid_postfix("a") == 0);
    Require(uprv_pathIsAbsolute_nid_postfix("relative/path") == 0);
    Require(uprv_pathIsAbsolute_nid_postfix("./current") == 0);
}

void TimeChecks() {
    const double first = uprv_getRawUTCtime_nid_postfix();
    const double throughWrapper = uprv_getUTCtime_nid_postfix();
    const double second = uprv_getRawUTCtime_nid_postfix();
    Require(first > 0 && throughWrapper > 0);
    Require(second >= first);
    Require(second - first < 60000);
}

void LocaleChecks() {
    UnsetLocaleEnvironment();
    u_cleanup_nid_postfix();
    ExpectLocale("en_US_POSIX");

    Set("LC_ALL", "ab_CD.EF@GH");
    u_cleanup_nid_postfix();
    ExpectLocale("ab_CD_GH");

    Set("LC_ALL", "ab@CD.EF");
    u_cleanup_nid_postfix();
    ExpectLocale("ab__CD");

    Set("LC_ALL", "no@nynorsk");
    u_cleanup_nid_postfix();
    ExpectLocale("no__NY");

    Set("LC_ALL", "ab_CD@ZZ.EF@GH.IJ");
    u_cleanup_nid_postfix();
    ExpectLocale("ab_CD_GH");

    Set("LC_ALL", "POSIX.UTF-8");
    u_cleanup_nid_postfix();
    ExpectLocale("en_US_POSIX");

    Set("LC_ALL", "C");
    u_cleanup_nid_postfix();
    ExpectLocale("en_US_POSIX");

    Set("LC_ALL", "C.UTF-8");
    u_cleanup_nid_postfix();
    ExpectLocale("en_US_POSIX");

    Set("LC_ALL", "nb_NO.UTF-8");
    u_cleanup_nid_postfix();
    ExpectLocale("nb_NO");
    Set("LC_ALL", "da_DK.UTF-8");
    ExpectLocale("nb_NO");
    u_cleanup_nid_postfix();
    ExpectLocale("da_DK");

    Require(unsetenv_nid_postfix("LC_ALL") == 0);
    Set("LC_MESSAGES", "fr_FR.UTF-8");
    u_cleanup_nid_postfix();
    ExpectLocale("fr_FR");

    Require(unsetenv_nid_postfix("LC_MESSAGES") == 0);
    Set("LANG", "de_DE.UTF-8");
    u_cleanup_nid_postfix();
    ExpectLocale("de_DE");
}

}

int main() {
    PathChecks();
    TimeChecks();
    LocaleChecks();
    return 0;
}
