#ifndef _DUST_CORE_PROFILING_HPP_
#define _DUST_CORE_PROFILING_HPP_

#ifdef DUST_PROFILING
    #include "tracy/Tracy.hpp"
    #define DUST_PROFILE ZoneScoped
    #define DUST_PROFILE_FRAME(x) FrameMarkNamed(x)
    #define DUST_PROFILE_SECTION(x) ZoneScopedN(x)
    #define DUST_PROFILE_TAG(y, x) ZoneText(x, strlen(x))
    #define DUST_PROFILE_LOG(text, size) TracyMessage(text, size)
    #define DUST_PROFILE_VALUE(text, value) TracyPlot(text, value)

    #include "../render/renderAPI.hpp"
    #include "tracy/TracyOpenGL.hpp"
    #define DUST_PROFILE_GPU_SETUP TracyGpuContext
    #define DUST_PROFILE_GPU(x) TracyGpuZone(x)
    #define DUST_PROFILE_GPU_COLLECT TracyGpuCollect
#else
    #define DUST_PROFILE
    #define DUST_PROFILE_FRAME(x)
    #define DUST_PROFILE_SECTION(x)
    #define DUST_PROFILE_TAG(y,x)
    #define DUST_PROFILE_LOG(text, size)
    #define DUST_PROFILE_VALUE(text, value)

    #define DUST_PROFILE_GPU_SETUP
    #define DUST_PROFILE_GPU(x)
    #define DUST_PROFILE_GPU_COLLECT
#endif

#endif //_DUST_CORE_PROFILING_HPP_