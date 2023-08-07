#ifndef _DUST_CORE_LOG_HPP_
#define _DUST_CORE_LOG_HPP_

#include <spdlog/spdlog.h>

#define DUST_ERROR(...) spdlog::error(__VA_ARGS__)
#define DUST_INFO(...) spdlog::info(__VA_ARGS__)
#define DUST_WARN(...) spdlog::warn(__VA_ARGS__)
#define DUST_TRACE(...) spdlog::trace(__VA_ARGS__)

#endif //_DUST_CORE_LOG_HPP_