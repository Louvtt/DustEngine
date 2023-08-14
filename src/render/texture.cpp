#include "dust/render/texture.hpp"
#include "dust/core/log.hpp"
#include "dust/render/renderAPI.hpp"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace dr = dust::render;

dr::Texture::Texture(const Desc& descriptor) 
: m_channels(descriptor.channels),
m_height(descriptor.height), m_width(descriptor.width),
m_lastIndex(0),
m_renderID(0)
{
    internalCreate(descriptor);
}

dr::Texture::Texture(const std::string& path)
: m_renderID(0)
{
    if(!std::filesystem::exists(path)) {
        DUST_ERROR("[Texture] {} doesn't exists.", path);
        return;
    }

    int width, height, nrChannels;
    u8* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if(!data) {
        DUST_ERROR("[Texture][StbImage] Failed to load image : {}", stbi_failure_reason());
        return;
    }

    internalCreate({
        data, (u32)width, (u32)height, (u32)nrChannels
    });
}

void dr::Texture::internalCreate(const Desc& descriptor)
{
    glGenTextures(1, &m_renderID);
    if(m_renderID == 0) {
        DUST_ERROR("[OpenGL][Texture] Failed to create a texture.");
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_renderID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, descriptor.width, descriptor.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, descriptor.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, apiValue(descriptor.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, apiValue(descriptor.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, apiValue(descriptor.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, apiValue(descriptor.wrap));
    if(descriptor.mipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}
u32 dr::Texture::apiValue(Wrap wrap)
{
    switch(wrap) {
        case Wrap::NoWrap: return GL_CLAMP;
        case Wrap::Wrap:   return GL_REPEAT;
        case Wrap::Mirror: return GL_MIRRORED_REPEAT;
        default: return GL_CLAMP;
    }
}
u32 dr::Texture::apiValue(Filter filter)
{
    switch(filter) {
        case Filter::Point: return GL_NEAREST;
        case Filter::Linear: return GL_LINEAR;
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