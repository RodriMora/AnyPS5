#pragma once
#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdint>

extern "C" {
double APS5_VABI uprv_getUTCtime_nid_postfix();
double APS5_VABI uprv_getRawUTCtime_nid_postfix();
std::int8_t APS5_VABI uprv_pathIsAbsolute_nid_postfix(const char* path);
const char* APS5_VABI uprv_getDefaultLocaleID_nid_postfix();
}
