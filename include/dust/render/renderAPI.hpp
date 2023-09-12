#ifndef _DUST_RENDER_RENDERAPI_HPP_
#define _DUST_RENDER_RENDERAPI_HPP_

#if defined(DUST_USE_WEBGL)
#    warning Using WebGL
#    include <glad/wgl.h>
#else
#    include <glad/gl.h>
#endif

#endif  //_DUST_RENDER_RENDERAPI_HPP_