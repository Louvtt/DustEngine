#include "dust/render/mesh.hpp"

#include "dust/core/profiling.hpp"
#include "dust/render/renderAPI.hpp"
#include "dust/core/log.hpp"
#include "dust/render/shader.hpp"
#include <algorithm>

namespace dr = dust::render;

/**********************************************************/
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

/**********************************************************/

dr::Mesh::Mesh(void *vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<u32> indices, std::vector<Attribute> attributes)
: m_indexCount(indices.size()),
m_vertexCount(vertexCount),
m_materialSlots(),
m_name(),
m_hidden(false)
{
    DUST_PROFILE;
    m_materialSlots.fill(nullptr);
    DUST_PROFILE_GPU("CreateVertexArrays");
    glCreateVertexArrays(1, &m_renderID);
    if(m_renderID == 0) {
        DUST_ERROR("[OpenGL][Mesh] Failed to create VAO");
        return;
    }
    glBindVertexArray(m_renderID);

    glGenBuffers(1, &m_vbo);
    if(m_vbo == 0) {
        DUST_ERROR("[OpenGL][Mesh] Failed to create VBO");
        return;
    } else {
        DUST_DEBUG("[OpenGL][Mesh] Created VBO {}", m_vbo);
    }
    // VBO
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        DUST_PROFILE_GPU("BufferData (VBO)");
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize * vertexCount, vertexData, GL_STATIC_DRAW);
    }
    // EBO
    m_ebo = 0;
    if(m_indexCount > 0) {
        glGenBuffers(1, &m_ebo);
        if(m_ebo == 0) {
            DUST_ERROR("[OpenGL][Mesh] Failed to create EBO");
            return;
        } else {
            DUST_DEBUG("[OpenGL][Mesh] Created EBO {}", m_ebo);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        DUST_PROFILE_GPU("BufferData (EBO)");
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), &indices.front(), GL_STATIC_DRAW);
    }

    bindAttributes(attributes);
    DUST_DEBUG("[OpenGL] Created Mesh {}", m_renderID);
    glBindVertexArray(0);
}
dr::Mesh::Mesh(const std::vector<float> &vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<Attribute> attribute)
: dr::Mesh::Mesh((void*)&vertexData.front(), vertexDataSize, vertexCount, {}, attribute) {}
dr::Mesh::Mesh(void *vertexData, u32 vertexDataSize, u32 vertexCount, std::vector<Attribute> attribute)
: dr::Mesh::Mesh(vertexData, vertexDataSize, vertexCount, {}, attribute) {}

dr::Mesh::~Mesh()
{   
    DUST_PROFILE;
    glBindVertexArray(0);
    if(m_vbo) glDeleteBuffers(1, &m_vbo);
    if(m_ebo) glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_renderID);
}

void dr::Mesh::draw(const Shader *shader)
{
    DUST_PROFILE;
    if(m_hidden) return;
    
    u32 slot = 0;
    for(auto& material : m_materialSlots){
        if(material == nullptr) { slot++; continue; }
        material->bind(slot);
        slot++;
    }
    
    shader->use();    
    glBindVertexArray(m_renderID);
    if(m_ebo != 0) {
        DUST_PROFILE_GPU("DrawElements");
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    } else {
        DUST_PROFILE_GPU("DrawArrays");
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }
    glBindVertexArray(0);

    for(auto& material : m_materialSlots){
        if(material == nullptr) continue;
        material->unbind();
    }
}

std::array<dr::MaterialPtr, DUST_MATERIAL_SLOTS> dr::Mesh::getMaterials() const
{
    return m_materialSlots;
}

void dr::Mesh::bindAttributes(const std::vector<Attribute> &attributes)
{
    DUST_PROFILE_GPU("MeshAttribute");
    u32 stride = 0;
    std::for_each(attributes.begin(), attributes.end(), [&](const Attribute &attrib){
        stride += attrib.getSize();
    });
    // DUST_DEBUG("[OpenGL][VertexAttrib] Stride = {}", stride);

    u64 offset = 0;
    u32 index  = 0;
    for(const auto& attrib : attributes)
    {
        // DUST_DEBUG("[OpenGL][VertexAttrib] Vertex attrib {} : offset {}", index, offset);
        glVertexAttribPointer(index, attrib.getCount(), attrib.getGLType(), GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        offset += attrib.getSize();
        ++index;
    }
}

void dr::Mesh::setName(const std::string &name)
{
    m_name = name;
}
std::string dr::Mesh::getName() const
{
    return m_name;
}


void dr::Mesh::setHidden(bool hide)
{
    m_hidden = hide;
}
bool dr::Mesh::isHidden() const
{
    return m_hidden;
}

void dr::Mesh::setMaterial(u32 index, MaterialPtr material)
{
    DUST_PROFILE;
    if(index > DUST_MATERIAL_SLOTS) return;
    m_materialSlots[index] = material;
}
dr::MaterialPtr dr::Mesh::getMaterial(u32 index) const
{
    return m_materialSlots.at(index);
}

dr::MeshPtr
dr::Mesh::createPlane(glm::vec2 size, bool textureCoordinates)
{
    DUST_PROFILE;
    const glm::vec2 half = size*.5f;
    if(!textureCoordinates) {
        return createRef<Mesh>(std::vector<f32>{
            // pos                  // normal
            // first triangle
            -half.x, 0.f, +half.y,  0.f, 1.f, 0.f,
             half.x, 0.f, +half.y,  0.f, 1.f, 0.f,
             half.x, 0.f, -half.y,  0.f, 1.f, 0.f,

            // second triangle
             half.x, 0.f, -half.y,  0.f, 1.f, 0.f,
            -half.x, 0.f, -half.y,  0.f, 1.f, 0.f,
            -half.x, 0.f, +half.y,  0.f, 1.f, 0.f,
        }, sizeof(f32) * 6, 6, std::vector<Attribute>{ Attribute::Pos3D, Attribute::Pos3D });
    } else {
        return createRef<Mesh>(std::vector<f32>{
            // pos                 //normal       // tex
            // first triangle
            -half.x, 0.f, +half.y, 0.f, 1.f, 0.f,  0.f, 1.f,
             half.x, 0.f, +half.y, 0.f, 1.f, 0.f,  1.f, 1.f,
             half.x, 0.f, -half.y, 0.f, 1.f, 0.f,  1.f, 0.f,

            // second triangle
             half.x, 0.f, -half.y, 0.f, 1.f, 0.f,  1.f, 0.f,
            -half.x, 0.f, -half.y, 0.f, 1.f, 0.f,  0.f, 0.f,
            -half.x, 0.f, +half.y, 0.f, 1.f, 0.f,  0.f, 1.f,
        }, sizeof(f32) * 8, 6, std::vector<Attribute>{ Attribute::Pos3D, Attribute::Pos3D, Attribute::TexCoords});
    }
}

dr::MeshPtr
dr::Mesh::createCube(glm::vec3 size, bool textureCoordinates)
{
    DUST_PROFILE;
    const glm::vec3 half = size*.5f;
    if(!textureCoordinates) {
        return createRef<Mesh>(std::vector<f32>{
            // position           // normals
            // back face
             half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, // bottom-right         
             half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, // top-right
            -half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, // bottom-left
            -half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, // top-left
            -half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, // bottom-left
             half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, // top-right
            // front face
             half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, // top-right
             half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, // bottom-right
            -half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, // bottom-left
            -half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, // bottom-left
            -half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, // top-left
             half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, // top-right
            // left face
            -half.x, +half.y, -half.z, -1.0f,  0.0f,  0.0f, // bottom-left
            -half.x, -half.y, -half.z, -1.0f,  0.0f,  0.0f, // top-left
            -half.x, -half.y,  half.z, -1.0f,  0.0f,  0.0f, // top-right
            -half.x, -half.y,  half.z, -1.0f,  0.0f,  0.0f, // top-right
            -half.x, +half.y,  half.z, -1.0f,  0.0f,  0.0f, // bottom-right
            -half.x, +half.y, -half.z, -1.0f,  0.0f,  0.0f, // bottom-left
            // right face
             half.x, -half.y, -half.z,  1.0f,  0.0f,  0.0f, // top-right         
             half.x, +half.y, -half.z,  1.0f,  0.0f,  0.0f, // bottom-right
             half.x, -half.y,  half.z,  1.0f,  0.0f,  0.0f, // top-left
             half.x, +half.y,  half.z,  1.0f,  0.0f,  0.0f, // bottom-left     
             half.x, -half.y,  half.z,  1.0f,  0.0f,  0.0f, // top-left
             half.x, +half.y, -half.z,  1.0f,  0.0f,  0.0f, // bottom-right
            // top face
             half.x, +half.y,  half.z,  0.0f, +1.0f,  0.0f, // bottom-left
             half.x, +half.y, -half.z,  0.0f, +1.0f,  0.0f, // top-left
            -half.x, +half.y, -half.z,  0.0f, +1.0f,  0.0f, // top-right
            -half.x, +half.y, -half.z,  0.0f, +1.0f,  0.0f, // top-right
            -half.x, +half.y,  half.z,  0.0f, +1.0f,  0.0f, // bottom-right
             half.x, +half.y,  half.z,  0.0f, +1.0f,  0.0f, // bottom-left
            // bottom face
             half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, // top-right     
             half.x, -half.y , half.z,  0.0f, -1.0f,  0.0f, // bottom-right
            -half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, // top-left
            -half.x, -half.y,  half.z,  0.0f, -1.0f,  0.0f, // bottom-left        
            -half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, // top-left
             half.x, -half.y,  half.z,  0.0f, -1.0f,  0.0f, // bottom-right
        }, sizeof(f32) * 6, 36, std::vector<Attribute>{ Attribute::Pos3D, Attribute::Pos3D });
    } else {
        return createRef<Mesh>(std::vector<f32>{
            // position           // normals          // tex coords
            // back face
            -half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -half.x, +half.y, -half.z,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -half.x, -half.y, -half.z,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -half.x, -half.y,  half.z,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -half.x, +half.y,  half.z,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -half.x, -half.y,  half.z, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -half.x, -half.y, -half.z, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -half.x, +half.y, -half.z, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -half.x, +half.y, -half.z, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -half.x, +half.y,  half.z, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -half.x, -half.y,  half.z, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             half.x, -half.y,  half.z,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             half.x, +half.y, -half.z,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             half.x, -half.y, -half.z,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             half.x, +half.y, -half.z,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             half.x, -half.y,  half.z,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             half.x, +half.y,  half.z,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // top face
            -half.x, +half.y, -half.z,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-right
             half.x, +half.y, -half.z,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-left
             half.x, +half.y,  half.z,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             half.x, +half.y,  half.z,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -half.x, +half.y,  half.z,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -half.x, +half.y, -half.z,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // bottom face
            -half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-left
             half.x, -half.y , half.z,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             half.x, -half.y,  half.z,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -half.x, -half.y, -half.z,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -half.x, -half.y,  half.z,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        }, sizeof(f32) * 8, 36, std::vector<Attribute>{ Attribute::Pos3D, Attribute::Pos3D, Attribute::TexCoords });
    }
}