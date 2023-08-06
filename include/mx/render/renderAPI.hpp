#ifndef _MX_RENDER_RENDERAPI_HPP_
#define _MX_RENDER_RENDERAPI_HPP_

#if defined(MX_USE_VULKAN)
#   warning Using Vulkan
#   include <glad/vulkan.h>
#elif defined(MX_USE_WEBGL)
#   warning Using WebGL
#   include <glad/wgl.h>
#else
#   include <glad/gl.h>
#endif


#endif //_MX_RENDER_RENDERAPI_HPP_