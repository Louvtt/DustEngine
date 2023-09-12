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
        COLOR, COLOR_RGBA, COLOR_SRGB, COLOR_HDR,
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

    /**
     * @brief Resize framebuffer (recreates it by default)
     */
    void resize(u32 width, u32 height, bool recreate = true);


    void bind();
    void unbind();

    u32 getWidth() const;
    u32 getHeight() const;

    void bindAttachment(u32 bindIndex, AttachmentType type, u32 index = 0);
    Result<Attachment> getAttachment(AttachmentType attachment, u32 index = 0);

private:
    void deleteInternal(u32 renderID, const std::vector<Attachment> &attachments);
    void createInternal();
};

using FramebufferPtr = Ref<Framebuffer>;
using FramebufferUPtr = Scope<Framebuffer>;

}
}

#endif //_DUST_RENDER_FRAMEBUFFER_HPP_