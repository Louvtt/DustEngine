#ifndef _DUST_CORE_PLATFORM_HPP_
#define _DUST_CORE_PLATFORM_HPP_

#if defined(WIN32) || defined(_WIN32)
#   define _DUST_PLATFORM_WINDOWS
#elif defined(__unix__) || defined(__linux__)
#   define _DUST_PLATFORM_LINUX
#elif defined(__macos__)
#   define _DUST_PLATFORM_MACOS
#endif

#endif //_DUST_CORE_PLATFORM_HPP_