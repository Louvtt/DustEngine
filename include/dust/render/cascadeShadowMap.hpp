#ifndef _DUST_RENDER_CASCADESHADOWMAP_HPP_
#define _DUST_RENDER_CASCADESHADOWMAP_HPP_

#include "../core/types.hpp"
#include "dust/render/camera.hpp"
#include "dust/render/framebuffer.hpp"

namespace dust {
namespace render {

class CascadeShadowMap {
protected:
    Camera3DPtr m_camera;
};

} // namespace render
} // namespace dust

#endif //_DUST_RENDER_CASCADESHADOWMAP_HPP_s