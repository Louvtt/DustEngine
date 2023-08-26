#include "dust/render/framebuffer.hpp"
#include "dust/core/types.hpp"
#include "dust/core/window.hpp"
#include "dust/render/renderAPI.hpp"
#include "dust/core/log.hpp"
#include <GL/gl.h>
#include <GL/glext.h>
#include <algorithm>
#include <type_traits>

namespace dr = dust::render;
using drf = dr::Framebuffer;

//////////////////////////////////////////
/// OpenGL Utils

inline static u32 getGLAttachment(const drf::AttachmentType& type)
{
    switch(type) {
        case drf::AttachmentType::DEPTH32:
        case drf::AttachmentType::DEPTH:
            return GL_DEPTH_ATTACHMENT;
        
        case drf::AttachmentType::STENCIL:
            return GL_STENCIL_ATTACHMENT;
        
        case drf::AttachmentType::DEPTH_STENCIL:
        case drf::AttachmentType::DEPTH32_STENCIL:
            return GL_DEPTH_STENCIL_ATTACHMENT;

        case drf::AttachmentType::COLOR:
        case drf::AttachmentType::COLOR_RGBA:
        case drf::AttachmentType::COLOR_SRGB:
        default:
            return GL_COLOR_ATTACHMENT0;
    }
}

inline static u32 getGLFormat(const drf::AttachmentType& type)
{
    switch (type) {
        case drf::AttachmentType::DEPTH:   return GL_DEPTH_COMPONENT;
        case drf::AttachmentType::DEPTH32: return GL_DEPTH_COMPONENT;
        
        case drf::AttachmentType::STENCIL: return GL_DEPTH_STENCIL;
        
        case drf::AttachmentType::DEPTH_STENCIL:   return GL_DEPTH_STENCIL;
        case drf::AttachmentType::DEPTH32_STENCIL: return GL_DEPTH_STENCIL;

        case drf::AttachmentType::COLOR:        return GL_RGB;
        case drf::AttachmentType::COLOR_RGBA:   return GL_RGBA;
        case drf::AttachmentType::COLOR_SRGB:   return GL_SRGB;
    }
} 

inline static u32 getGLType(const drf::AttachmentType& type)
{
    switch (type) {
        case drf::AttachmentType::DEPTH:   return GL_FLOAT;
        case drf::AttachmentType::DEPTH32: return GL_DOUBLE;
        
        case drf::AttachmentType::STENCIL: return GL_UNSIGNED_BYTE;
        
        case drf::AttachmentType::DEPTH_STENCIL:   return GL_FLOAT;
        case drf::AttachmentType::DEPTH32_STENCIL: return GL_DOUBLE;

        case drf::AttachmentType::COLOR:        return GL_UNSIGNED_BYTE;
        case drf::AttachmentType::COLOR_RGBA:   return GL_UNSIGNED_BYTE;
        case drf::AttachmentType::COLOR_SRGB:   return GL_UNSIGNED_BYTE;
    }
} 

//////////////////////////////////////////
/// Framebuffer

drf::Framebuffer(const drf::Framebuffer::Desc& desc)
: m_width(desc.width),
m_height(desc.height),
m_renderID(0),
m_colorAttachmentCount(0)
{
    glGenFramebuffers(1, &m_renderID);
    if(m_renderID == 0) {
        DUST_ERROR("[OpenGL][Framebuffer] Failed to generate a framebuffer");
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderID);

    // attachments
    for(const auto& attachment : desc.attachments)
    {
        Attachment newAttachment{0, attachment.type, 0, attachment.readable};
        if(attachment.type == drf::AttachmentType::COLOR
        || attachment.type == drf::AttachmentType::COLOR_RGBA
        || attachment.type == drf::AttachmentType::COLOR_SRGB) {
            newAttachment.index = m_colorAttachmentCount++;
        }

        // texture
        u32 format  = getGLFormat(attachment.type);
        u32 binding = getGLAttachment(attachment.type) + (u32)newAttachment.index;
        if(attachment.readable) {
            glGenTextures(1, &newAttachment.id);
            if(newAttachment.id == 0) { DUST_ERROR("[OpenGL][Framebuffer] Failed to create texture."); continue; }
            glBindTexture(GL_TEXTURE_2D, newAttachment.id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, getGLType(attachment.type), nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, 
                binding,
                GL_TEXTURE_2D,
                newAttachment.id,
                0
            );
            DUST_DEBUG("[OpenGL][Framebuffer] Create texture {} [index {}]", newAttachment.id, newAttachment.index);
        }
        // renderbuffer 
        else { 
            glGenRenderbuffers(1, &newAttachment.id);
            if(newAttachment.id == 0) { DUST_ERROR("[OpenGL][Framebuffer] Failed to create renderbuffer."); continue; }
            glBindRenderbuffer(GL_RENDERBUFFER, newAttachment.id);
            glRenderbufferStorage(GL_RENDERBUFFER, format, m_width, m_height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, binding, GL_RENDERBUFFER, newAttachment.id);
            DUST_DEBUG("[OpenGL][Framebuffer] Create renderbuffer {}", newAttachment.id);
        }
        m_attachments.push_back(newAttachment);
    }

    // No color buffer
    if(m_colorAttachmentCount == 0) 
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        DUST_DEBUG("[OpenGL] Created framebuffer {}", m_renderID);
    } else {
        DUST_ERROR("[OpenGL] Error while creating framebuffer {}", m_renderID);
        // delete everything
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for(auto attachment : m_attachments) {
            if(attachment.isReadable) { glDeleteTextures(1, &attachment.id); }
            else                      { glDeleteRenderbuffers(1, &attachment.id); }
        }
        m_attachments.clear();
        glDeleteFramebuffers(1, &m_renderID);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

drf::~Framebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_renderID);
}

void drf::resize(u32 width, u32 height)
{
    m_width  = width;
    m_height = height;
    for(auto attachment : m_attachments) {
        u32 format  = getGLFormat(attachment.type);
        if(attachment.isReadable) {
            glBindTexture(GL_TEXTURE_2D, attachment.id);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            glBindRenderbuffer(GL_RENDERBUFFER, attachment.id);
            glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }
}

void drf::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderID);
}
void drf::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

u32 drf::getWidth() const
{
    return m_width;
}
u32 drf::getHeight() const
{
    return m_height;
}  

dust::Result<drf::Attachment> 
drf::getAttachment(AttachmentType type, u32 index)
{
    decltype(auto) found = std::find_if(m_attachments.begin(), m_attachments.end(), [=](Attachment value) -> bool {
        return value.type == type && value.index == index;
    });

    if(found == m_attachments.end()) {
        return {};
    }
    return *found;
}

void drf::bindAttachment(u32 bindIndex, AttachmentType type, u32 index)
{
    decltype(auto) found = this->getAttachment(type, index);
    if(found.has_value()) {
        glActiveTexture(GL_TEXTURE0+bindIndex);
        glBindTexture(GL_TEXTURE_2D, found.value().id);
    }
}

