#include "dust/render/texture.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/render/renderAPI.hpp"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace dr = dust::render;

dr::TexturePtr dr::Texture::s_nullTexture = nullptr;

static u32 toGLFormat(u32 channels)
{
    switch (channels) {
        case 4: return GL_RGBA;
        case 3: return GL_RGB;
        case 2: return GL_RG;
        case 1:
        case 0:
        default: return GL_RED;
    }
}

dr::Texture::Texture(const TextureDesc& descriptor) 
: m_channels(descriptor.channels),
m_height(descriptor.height), m_width(descriptor.width),
m_lastIndex(0),
m_renderID(0)
{
    // Generate null texture (white RGBA 1x1)
    if(s_nullTexture == nullptr) {
        const u32 white_pixel = 0xFFFFFFFF;
        s_nullTexture = createRef<Texture>(TextureDesc{
            .data     = (void*)(&white_pixel),
            .width    = 1,
            .height   = 1,
            .channels = 4,
            .filter   = TextureFilter::Point,
            .wrap     = TextureWrap::Wrap,
            .mipMaps  = false
        });
    }

    internalCreate(descriptor);
}

void dr::Texture::internalCreate(const TextureDesc& descriptor)
{
    glGenTextures(1, &m_renderID);
    if(m_renderID == 0) {
        DUST_ERROR("[OpenGL][Texture] Failed to create a texture.");
        return;
    }
    glBindTexture(GL_TEXTURE_2D, m_renderID);
    glTexImage2D(GL_TEXTURE_2D, 0, toGLFormat(descriptor.channels), descriptor.width, descriptor.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, descriptor.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, apiValue(descriptor.filter, descriptor.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, apiValue(descriptor.filter, descriptor.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, apiValue(descriptor.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, apiValue(descriptor.wrap));
    if(descriptor.mipMaps) {
        DUST_DEBUG("[OpenGL][Texture] Creating mipmaps...");
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    DUST_DEBUG("[OpenGL] Created Texture {}", m_renderID);
}
u32 dr::Texture::apiValue(TextureWrap wrap)
{
    switch(wrap) {
        case TextureWrap::NoWrap: return GL_CLAMP;
        case TextureWrap::Wrap:   return GL_REPEAT;
        case TextureWrap::Mirror: return GL_MIRRORED_REPEAT;
        default: return GL_CLAMP;
    }
}
u32 dr::Texture::apiValue(TextureFilter filter, bool mipMaps)
{
    switch(filter) {
        case TextureFilter::Point: return mipMaps ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
        case TextureFilter::Linear: return mipMaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        default: return GL_LINEAR;
    }
}

dr::Texture::~Texture()
{
    unbind();
    glDeleteTextures(1, &m_renderID);
}

void dr::Texture::bind(u16 index)
{
    m_lastIndex = index;
    glActiveTexture(GL_TEXTURE0 + m_lastIndex);
    glBindTexture(GL_TEXTURE_2D, m_renderID);
}
void dr::Texture::unbind()
{
    glActiveTexture(GL_TEXTURE0 + m_lastIndex);
    glBindTexture(GL_TEXTURE_2D, m_renderID);
}

u32 dr::Texture::getWidth() const
{
    return m_width;
}
u32 dr::Texture::getHeight() const
{
    return m_height;
}
u32 dr::Texture::getChannels() const
{
    return m_channels;
}

u32 dr::Texture::getRenderID() const
{
    return m_renderID;
}

dr::TexturePtr dr::Texture::getNullTexture()
{
    return s_nullTexture;
}