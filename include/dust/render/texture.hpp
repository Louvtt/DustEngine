#ifndef _DUST_RENDER_TEXTURE_HPP_
#define _DUST_RENDER_TEXTURE_HPP_

#include "../core/types.hpp"
#include <vector>

namespace dust {

namespace render {

enum class TextureFilter : int {
    Point,
    Linear,
};

enum class TextureWrap : int {
    ClampEdge,
    NoWrap = ClampEdge,
    ClampBorder,
    Wrap,
    Mirror,
};

struct TextureParam {
  TextureFilter filter = TextureFilter::Point;
  TextureWrap   wrap   = TextureWrap::NoWrap;
  bool          mipMaps = true;
};

class Texture
{
protected:
    u32 m_renderID;

    u16 m_lastIndex;
    u32 m_width, m_height;
    u32 m_channels;

    static Ref<Texture> s_nullTexture;
private:
    /**
     * @brief Set every default, see static creators to create a texture.
     */
    Texture(u32 width, u32 height, u32 channels);

public:
    /**
     * @brief Delete the texture and the allocated gpu texture
     */
    ~Texture();

    void bind(u16 index = 0);
    void unbind();

    u32 getWidth() const;
    u32 getHeight() const;
    u32 getChannels() const;

    u32 getRenderID() const;

    static Ref<Texture> GetNullTexture();

    /**
     * @brief Create Texture 2D
     */
    static Ref<Texture> CreateTexture2D(u32 width, u32 height, u32 channels, void* data, const TextureParam& param);
    /**
     * @brief Create Texture 2D with mipMaps layers
     * @param width Width of the Texture
     * @param height Height of the Texture
     * @param channels Channels count of the texture
     * @param data List of the data for each mipmaps level
     */
    static Ref<Texture> CreateTexture2D(u32 width, u32 height, u32 channels, std::vector<void*> data, const TextureParam& param);

    /**
     * @brief Create Texture 2D with mipMaps layers
     * @param width Width of the Texture
     * @param height Height of the Texture
     * @param channels Channels count of the texture
     * @param data List of the data for each mipmaps level
     */
    static Ref<Texture> CreateTextureCompressed2D(u32 width, u32 height, u32 channels, u32 size, std::vector<void*> data, const TextureParam& param);

    static Ref<Texture> CreateTextureCubeMap(u32 width, u32 height, u32 channels, std::vector<void*> faces, const TextureParam& param);
    static Ref<Texture> CreateTexture2DArray(u32 width, u32 height, u32 channels, std::vector<void*>, const TextureParam& param);

    /**
     * @brief Create Texture manually, just allocate the gpu texture type for you to use
     */
    static Ref<Texture> CreateTextureRaw(int apiType, u32 width, u32 height, u32 channels);

private:
    bool internalCreate(u32 apiTextureType);
    static u32 apiValue(TextureWrap wrap);
    static u32 apiValue(TextureFilter filter, bool mipMaps);
};

using TexturePtr = Ref<Texture>;
using TextureUPtr = Scope<Texture>;

}
}

#endif //_DUST_RENDER_TEXTURE_HPP_
