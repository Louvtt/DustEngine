#ifndef _DUST_RENDER_TEXTURE_HPP_
#define _DUST_RENDER_TEXTURE_HPP_

#include "../core/types.hpp"

namespace dust {

namespace render {

enum class TextureFilter : int {
    Point,
    Linear,
};

enum class TextureWrap : int {
    ClampEdge,
    ClampBorder,
    NoWrap = ClampEdge,
    Wrap,
    Mirror,
};

struct TextureDesc {
    void* data;
    u32 width;
    u32 height;
    u32 channels;
    TextureFilter filter = TextureFilter::Linear;
    TextureWrap wrap = TextureWrap::NoWrap;
    bool mipMaps = false;
};

class Texture
{
protected:
    u32 m_renderID;

    u16 m_lastIndex;
    u32 m_width, m_height;
    u32 m_channels;

    static Ref<Texture> s_nullTexture;
public:
    
    Texture(const TextureDesc& descriptor);
    ~Texture();

    void bind(u16 index = 0);
    void unbind();

    u32 getWidth() const;
    u32 getHeight() const;
    u32 getChannels() const;

    u32 getRenderID() const;

    static Ref<Texture> GetNullTexture();

private:
    void internalCreate(const TextureDesc& descriptor);
    static u32 apiValue(TextureWrap wrap);
    static u32 apiValue(TextureFilter filter, bool mipMaps);
};

using TexturePtr = Ref<Texture>;
using TextureUPtr = Scope<Texture>;

}
}

#endif //_DUST_RENDER_TEXTURE_HPP_