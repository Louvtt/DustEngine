#ifndef _DUST_RENDER_FRAMEBUFFER_HPP_
#define _DUST_RENDER_FRAMEBUFFER_HPP_

#include "../core/types.hpp"
#include "dust/render/texture.hpp"

#include <vector>

namespace dust {
namespace render {

class Framebuffer
{
public:
    enum class AttachmentType : u16 {
        /// Color attachment
        COLOR, COLOR_RGBA, COLOR_SRGB,
        /// Depth attachment
        DEPTH, DEPTH32, 
        /// Stencil attachment
        STENCIL, DEPTH_STENCIL, DEPTH32_STENCIL
    };

    struct AttachmentDesc {
        AttachmentType type;
        bool readable = false;
    };

    struct Attachment {
        u32 id;
        AttachmentType type;
        u32 index;
        bool isReadable;
    };

    struct Desc {
        std::vector<AttachmentDesc> attachments;
        u32 width;
        u32 height;
    };

protected:
    u32 m_renderID;
    std::vector<Attachment> m_attachments;
    u32 m_colorAttachmentCount;
    u32 m_width, m_height;
    
public:
    Framebuffer(const Desc& desc);
    ~Framebuffer();

    void resize(u32 width, u32 height);
    void bind();
    void unbind();

    Result<Attachment> getAttachment(AttachmentType attachment, u32 index = 0);
};

}
}

#endif //_DUST_RENDER_FRAMEBUFFER_HPP_