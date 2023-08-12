#include "dust/render/mesh.hpp"
#include "dust/render/renderAPI.hpp"
#include "dust/core/log.hpp"
#include <algorithm>

namespace dr = dust::render;

/* ===================== */
// Attribute

dr::Attribute::Attribute(u32 count, u32 size, u32 glType)
: m_count(count), m_size(size), m_glType(glType) {}

u32 dr::Attribute::getSize()   const { return m_size;   }
u32 dr::Attribute::getCount()  const { return m_count;  }
u32 dr::Attribute::getGLType() const { return m_glType; }

dr::Attribute dr::Attribute::Float     {1, 1 * sizeof(float), GL_FLOAT};
dr::Attribute dr::Attribute::Float2    {2, 2 * sizeof(float), GL_FLOAT};
dr::Attribute dr::Attribute::Float3    {3, 3 * sizeof(float), GL_FLOAT};
dr::Attribute dr::Attribute::Float4    {4, 4 * sizeof(float), GL_FLOAT};
dr::Attribute dr::Attribute::TexCoords {Float2};
dr::Attribute dr::Attribute::Pos2D     {Float2};
dr::Attribute dr::Attribute::Pos3D     {Float3};
dr::Attribute dr::Attribute::Color     {Float4};

/* ==================== */

dr::Mesh::Mesh(const dr::MeshDescriptor& descriptor)
: m_indexCount(descriptor.indices.size()),
m_vertexCount(descriptor.vertexCount)
{
    glCreateVertexArrays(1, &m_renderID);
    glBindVertexArray(m_renderID);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, descriptor.verticesData.size() * sizeof(float), &descriptor.verticesData.front(), GL_STATIC_DRAW);

    if(m_indexCount > 0) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, descriptor.indices.size() * sizeof(float), &descriptor.indices.front(), GL_STATIC_DRAW);
    }

    bindAttributes(descriptor.attributes);
    DUST_DEBUG("[OpenGL] Created Mesh {}", m_renderID);
    glBindVertexArray(0);
}
dr::Mesh::~Mesh()
{
    glBindVertexArray(0);
    if(m_vbo) glDeleteBuffers(1, &m_vbo);
    if(m_ebo) glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_renderID);
}

void dr::Mesh::draw()
{
    glBindVertexArray(m_renderID);
    if(m_ebo == 0) {
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
    glBindVertexArray(0);
}

void dr::Mesh::bindAttributes(const std::vector<Attribute> &attributes)
{
    u32 stride = 0;
    std::for_each(attributes.begin(), attributes.end(), [&](const Attribute &attrib){
        stride += attrib.getSize();
    });
    DUST_DEBUG("[OpenGL][VertexAttrib] Stride = {}", stride);

    u64 offset = 0;
    u32 index  = 0;
    for(const auto& attrib : attributes)
    {
        DUST_DEBUG("[OpenGL][VertexAttrib] Vertex attrib {} : offset {}", index, offset);
        glVertexAttribPointer(index, attrib.getCount(), attrib.getGLType(), GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        offset += attrib.getSize();
        ++index;
    }
}