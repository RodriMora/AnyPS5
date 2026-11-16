# libSceIcu

## change [ICU buffer termination]

- [ICU buffer termination](../../../core/libs/prx/libSceIcu/TerminateBuffer.cpp) has no console ABI or cross-firmware verification. `u_terminateChars`, `u_terminateUChar32s`, `u_terminateUChars` and `u_terminateWChars` share one ICU 68.2 termination contract: success writes a terminator when there is room, an exact fit reports `U_STRING_NOT_TERMINATED_WARNING`, excess reports `U_BUFFER_OVERFLOW_ERROR`. A null destination throws only when a terminator would be written. Closes when: compared with the console.

## add Functional

- [Platform helpers](../../../core/libs/prx/libSceIcu/Platform.cpp) have no console ABI or behavior verification; the behavior follows ICU 78.1 [putil.cpp](https://github.com/unicode-org/icu/blob/049e0d6a420629ac7db77256987d083a563287b5/icu4c/source/common/putil.cpp). `uprv_getUTCtime` forwards to `uprv_getRawUTCtime`, milliseconds since the epoch as a `UDate` double. `uprv_pathIsAbsolute` takes the POSIX branch: false for null or empty, true for a leading `/`. `uprv_getDefaultLocaleID` takes the POSIX correction: strip the codepage and the first `@` onward, map `C` and `POSIX` to `en_US_POSIX`, then re-append the rightmost `@variant`, `__` when the base has no `_` (`nynorsk` becomes `NY`) else `_`, without its codepage. The source locale is the guest `setlocale` query, always `C`, so it falls back to `LC_ALL`, `LC_MESSAGES`, `LANG`, then `en_US_POSIX`; an empty `LC_ALL` counts, as upstream does off Solaris. `u_cleanup` drops the cached result. Closes when: compared with the console.
