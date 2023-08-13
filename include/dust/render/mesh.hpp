#ifndef _DUST_RENDER_MESH_HPP_
#define _DUST_RENDER_MESH_HPP_

#include "dust/core/types.hpp"
#include <vector>

namespace dust {
namespace render {

class Attribute 
{
private:
    u32 m_count;
    u32 m_size;
    u32 m_glType;
    Attribute(u32 count, u32 size, u32 glType);

public:
    u32 getSize() const;
    u32 getCount() const;
    u32 getGLType() const;

    static Attribute Float;
    static Attribute Float2;
    static Attribute Pos2D;
    static Attribute TexCoords;
    static Attribute Float3;
    static Attribute Pos3D;
    static Attribute Float4;
    static Attribute Color;
};

class Mesh 
{
protected:
    u32 m_renderID;

    u32 m_vbo;
    u32 m_ebo;

    u32 m_indexCount;
    u32 m_vertexCount;

public:
    Mesh(std::vector<float> vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<Attribute> attributes);
    Mesh(void* vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<Attribute> attributes);
    Mesh(void* vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<u32> indices, std::vector<Attribute> attributes);
    ~Mesh();

    void draw();

protected:
    void bindAttributes(const std::vector<Attribute> &attributes);
};

}
}

#endif //_DUST_RENDER_MESH_HPP_