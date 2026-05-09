#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DDPROTO_VERSIONSTR "v0.0.1"

#define DDPROTO_DATE "built on " __DATE__ " " __TIME__

#define DDPROTO_XSTR(s) #s
#define DDPROTO_STR(s) DDPROTO_XSTR(s)

#ifdef __clang__
#define DDPROTO_COMPILER "clang " __clang_version__
#elif __GNUC__
#define DDPROTO_COMPILER "gcc " __VERSION__
#elif _MSC_VER
#define DDPROTO_COMPILER "MSVC " DDPROTO_STR(_MSC_FULL_VER)
#else
#define DDPROTO_COMPILER "unknown"
#endif

#define DDPROTO_BUILDINFO DDPROTO_VERSIONSTR ", " DDPROTO_DATE ", " DDPROTO_COMPILER

#ifdef __cplusplus
}
#endif
