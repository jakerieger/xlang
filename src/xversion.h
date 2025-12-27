#ifndef X_VERSION_H
#define X_VERSION_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0
#define VERSION_STRING "0.1.0"

#ifdef __linux
    #define VERSION_STRING_FULL VERSION_STRING "-linux-x64"
#elif _WIN32
    #define VERSION_STRING_FULL VERSION_STRING "-windows-x64"
#elif __darwin__
    #define VERSION_STRING_FULL VERSION_STRING "-macos-x64"
#endif

#endif
