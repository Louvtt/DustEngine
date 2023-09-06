#ifndef _DUST_RENDER_SHADER_HPP_
#define _DUST_RENDER_SHADER_HPP_

#include "../core/types.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>

#ifdef EMSCRIPTEN
#define SHADER_PREFIX "precision highp float"
#else
#define SHADER_PREFIX ""
#endif

#define SHADER_SOURCE(shader_version, ...) shader_version "\n" #__VA_ARGS__

namespace dust {

namespace render {

/**
 * @brief Type of the uniform in a shader program
 */
enum class UniformType {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Bool,
    Mat4,
    Sampler2D,
    Sampler3D,
    SamplerCube,
};

/**
 * @brief Represent a shader uniform
 */
struct Uniform {
    /// Name of the uniform
    std::string name;
    /// Type of the uniform
    UniformType type;
    /// Index in the active uniforms
    u32 index;
};

/**
 * @brief Represent a OpenGL Shader
 */
class Shader
{
private:
    u32 m_renderID{0};

    std::string m_vertexFilePath{};
    std::string m_fragmentFilePath{};

    std::unordered_map<std::string, Uniform> m_uniforms;
public:
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    ~Shader();

    virtual void use();

    virtual void setUniform(const std::string &name, int value);
    virtual void setUniform(const std::string &name, bool value);
    virtual void setUniform(const std::string &name, float value);
    virtual void setUniform(const std::string &name, glm::vec2 value);
    virtual void setUniform(const std::string &name, glm::vec3 value);
    virtual void setUniform(const std::string &name, glm::vec4 value);
    virtual void setUniform(const std::string &name, glm::mat4 value);

    virtual void reload();
    static Result<Ref<Shader>> loadFromFile(const std::string &vertexPath, const std::string &fragmentPath);

protected:
    Shader();

private:
    u32 internalCreate(const std::string &vertexCode, const std::string &fragmentCode);
    u32 getUniformLocation(const std::string &name);
    u32 compileShader(int type, const std::string& code);
    bool linkShaders(u32 program, u32 vertexShader, u32 fragmentShader);
    bool validateProgram(u32 program);

    void queryActiveUniforms(u32 program);
};

using ShaderPtr = Ref<Shader>;
using ShaderUPtr = Scope<Shader>;

}

}

#endif //_DUST_RENDER_SHADER_HPP_