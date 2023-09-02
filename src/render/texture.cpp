#include "dust/render/texture.hpp"
#include "dust/core/log.hpp"
#include "dust/core/types.hpp"
#include "dust/render/renderAPI.hpp"
#include <GL/gl.h>
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

dr::Texture::Texture(u32 width, u32 height, u32 channels) 
: m_channels(channels),
m_height(height), m_width(width),
m_lastIndex(0),
m_renderID(0)
{ }


bool dr::Texture::internalCreate(u32 apiTextureType)
{
    glGenTextures(1, &m_renderID);
    if(m_renderID == 0) {
        DUST_ERROR("[OpenGL][Texture] Failed to create a texture.");
        return false;
    }
    return true;
}

dr::TexturePtr dr::Texture::CreateTexture2D(u32 width, u32 height, u32 channels, void* data, const TextureParam& param) 
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    if(!texture->internalCreate(GL_TEXTURE_2D)) {
      texture.reset();
      return GetNullTexture();
    }

    glBindTexture(GL_TEXTURE_2D, texture->m_renderID);
    glTexImage2D(GL_TEXTURE_2D, 0, toGLFormat(channels), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, apiValue(param.filter, param.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, apiValue(param.filter, param.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, apiValue(param.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, apiValue(param.wrap));
    if(param.mipMaps) {
        DUST_DEBUG("[OpenGL][Texture] Creating mipmaps...");
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    DUST_DEBUG("[OpenGL] Created Texture {}", texture->m_renderID);
    return texture;
}

dr::TexturePtr dr::Texture::CreateTexture2DArray(u32 width, u32 height, u32 channels, std::vector<void *>, const TextureParam &param)
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    if(!texture->internalCreate(GL_TEXTURE_2D_ARRAY)) {
        texture.reset();
        return GetNullTexture();
    }
    // TODO: Implement texture arrays
    DUST_WARN("[OpenGL] [Texture] Texture 2D Arrays creation is not implemented yet.");
    return texture;
}

dr::TexturePtr dr::Texture::CreateTextureRaw(int apiType, u32 width, u32 height, u32 channels)
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    texture->internalCreate(apiType);
    texture->bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    texture->unbind();
    return texture;
}

dr::TexturePtr dr::Texture::CreateTextureCubeMap(u32 width, u32 height, u32 channels, std::vector<void *> faces, const TextureParam &param)
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    if(!texture->internalCreate(GL_TEXTURE_CUBE_MAP)) {
        texture.reset();
        return GetNullTexture();
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture->m_renderID);
    u32 index = 0;
    for(auto face : faces)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 
             0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, face
        );
        ++index;
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, apiValue(param.filter, false));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, apiValue(param.filter, false));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, apiValue(param.wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, apiValue(param.wrap));
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, apiValue(param.wrap));
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return texture;

}

dr::TexturePtr dr::Texture::CreateTexture2D(u32 width, u32 height, u32 channels, std::vector<void *> data, const TextureParam &param)
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    if(!texture->internalCreate(GL_TEXTURE_2D)) {
        texture.reset();
        return GetNullTexture();
    }

    glBindTexture(GL_TEXTURE_2D, texture->m_renderID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, apiValue(param.filter, param.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, apiValue(param.filter, param.mipMaps));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, apiValue(param.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, apiValue(param.wrap));
    for(int i = 0; i < data.size(); ++i) {
        glTexImage2D(GL_TEXTURE_2D, i, toGLFormat(channels), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.at(i));
    }
    if(param.mipMaps) {
        DUST_DEBUG("[OpenGL][Texture] Creating mipmaps...");
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    DUST_DEBUG("[OpenGL] Created Texture {}", texture->m_renderID);
    return texture;
    return texture;
}

dr::TexturePtr dr::Texture::CreateTextureCompressed2D(u32 width, u32 height, u32 channels, u32 size, std::vector<void *> data, const TextureParam &param)
{
    TexturePtr texture = TexturePtr(new Texture(width, height, channels));
    if(!texture->internalCreate(GL_TEXTURE_2D)) {
        texture.reset();
        return GetNullTexture();
    }

    glBindTexture(GL_TEXTURE_2D, texture->m_renderID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, apiValue(param.filter, true));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, apiValue(param.filter, true));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, apiValue(param.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, apiValue(param.wrap));
    for(int i = 0; i < data.size(); ++i) {
        glCompressedTexImage2D(GL_TEXTURE_2D, i, toGLFormat(channels), width, height, 0, size, data.at(i));
    }
    if(param.mipMaps) {
        DUST_DEBUG("[OpenGL][Texture] Creating mipmaps...");
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    DUST_DEBUG("[OpenGL] Created Texture {}", texture->m_renderID);
    return texture;
}

u32 dr::Texture::apiValue(TextureWrap wrap)
{
    switch(wrap) {
        case TextureWrap::ClampEdge:   return GL_CLAMP_TO_EDGE;
        case TextureWrap::ClampBorder: return GL_CLAMP_TO_BORDER;
        case TextureWrap::Wrap:        return GL_REPEAT;
        case TextureWrap::Mirror:      return GL_MIRRORED_REPEAT;
        default: return GL_CLAMP_TO_EDGE;
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

dr::TexturePtr dr::Texture::GetNullTexture()
{
    // Generate null texture (white RGBA 1x1)
    if(s_nullTexture == nullptr) {
        const u32 white_pixel = 0xFFFFFFFF;
        s_nullTexture = CreateTexture2D(
            1,
            1,
            4,
            (void*)(&white_pixel),
            TextureParam {
                .filter   = TextureFilter::Point,
                .wrap     = TextureWrap::Wrap,
                .mipMaps  = false
            }
        );
        DUST_INFO("[Texture] Created Default white texture.");
    }
    return s_nullTexture;
}
