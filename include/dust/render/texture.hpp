#ifndef _DUST_RENDER_TEXTURE_HPP_
#define _DUST_RENDER_TEXTURE_HPP_

#include "../core/types.hpp"

namespace dust {

namespace render {

class Texture
{
protected:
    u32 m_renderID;

    u16 m_lastIndex;
    u32 m_width, m_height;
    u32 m_channels;
public:
    enum class Filter : int {
        Point,
        Linear,
    };
    enum class Wrap : int {
        NoWrap,
        Wrap,
        Mirror,
    };

    struct Desc {
        void* data;
        u32 width;
        u32 height;
        u32 channels;
        Filter filter = Filter::Linear;
        Wrap wrap = Wrap::NoWrap;
        bool mipMaps = false;
    };

    Texture(const Desc& descriptor);
    Texture(const std::string& path);
    ~Texture();

    void bind(u16 index = 0);
    void unbind();

    u32 getWidth() const;
    u32 getHeight() const;
    u32 getChannels() const;

private:
    void internalCreate(const Desc& descriptor);
    static u32 apiValue(Wrap wrap);
    static u32 apiValue(Filter filter);
};

}
}

#endif //_DUST_RENDER_TEXTURE_HPP_