#ifndef _DUST_RENDER_SHADER_HPP_
#define _DUST_RENDER_SHADER_HPP_

#include "../core/types.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#ifdef EMSCRIPTEN
#define SHADER_PREFIX "precision highp float"
#else
#define SHADER_PREFIX ""
#endif

#define SHADER_SOURCE(shader_version, ...) shader_version "\n" #__VA_ARGS__

namespace dust {

namespace render {

class Shader
{
private:
    u32 m_renderID{0};

    std::string m_vertexFilePath{};
    std::string m_fragmentFilePath{};
public:
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    ~Shader();

    virtual void use();

    template <typename UniformType>
    void setUniform(const std::string& name, UniformType value);

    template <int>         void setUniform(const std::string &name, int value);
    template <float>       void setUniform(const std::string &name, float value);
    template <glm::vec2>   void setUniform(const std::string &name, glm::vec2 value);
    template <glm::vec3>   void setUniform(const std::string &name, glm::vec3 value);
    template <glm::vec4>   void setUniform(const std::string &name, glm::vec4 value);
    
    void setUniformMat4(const std::string &name, glm::mat4 value);

    void reload();
    static Ref<Shader> loadFromFile(const std::string &vertexPath, const std::string &fragmentPath);

protected:
    Shader();

private:
    u32 getUniformLocation(const std::string &name);
    u32 compileShader(int type, const std::string& code);
    bool linkShaders(u32 vertexShader, u32 fragmentShader);
};

}

}

#endif //_DUST_RENDER_SHADER_HPP_