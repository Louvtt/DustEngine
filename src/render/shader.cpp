#include "dust/render/shader.hpp"

#include "dust/core/log.hpp"
#include "dust/io/fileReader.hpp"
#include "dust/render/renderAPI.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <fstream>
#include <system_error>

namespace fs = std::filesystem;
namespace dr = dust::render;

#define INFO_LOG_SIZE 1024
static char infoLog[INFO_LOG_SIZE];

dr::Shader::Shader(const std::string &vertexCode, const std::string &fragmentCode)
: m_renderID(0)
{
    u32 vertex   = compileShader(GL_VERTEX_SHADER, vertexCode);
    if(vertex == 0) { return; }
    u32 fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if(fragment == 0) { return; }

    m_renderID = glCreateProgram();
    if(m_renderID == 0) { return; }
    linkShaders(vertex, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    DUST_DEBUG("[OpenGL] Created Shader {}", m_renderID);
}

dr::Shader::~Shader()
{
    glDeleteProgram(m_renderID);
}

void dr::Shader::use() {
    glUseProgram(m_renderID);
}

template <typename UniformType>
void dr::Shader::setUniform(const std::string& name, UniformType value) {
    DUST_ERROR("[OpenGL] Cannot set an uniform of this type ({})", typeid(UniformType).name());
}

template <> 
void dr::Shader::setUniform(const std::string &name, int value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform1i(loc, value);
    }
}
template <> 
void dr::Shader::setUniform(const std::string &name, float value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform1f(loc, value);
    }
}
template <> 
void dr::Shader::setUniform(const std::string &name, glm::vec2 value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform2f(loc, value.x, value.y);
    }
}
template <> 
void dr::Shader::setUniform(const std::string &name, glm::vec3 value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform3f(loc, value.x, value.y, value.z);
    }
}
template <> 
void dr::Shader::setUniform(const std::string &name, glm::vec4 value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform4f(loc, value.x, value.y, value.z, value.w);
    }
}
void dr::Shader::setUniformMat4(const std::string &name, glm::mat4 value)
{
    u32 loc = getUniformLocation(name);
    if(loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void dr::Shader::reload() {}

dust::Ref<dr::Shader>
dr::Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::error_code error{};
    if(!fs::exists(vertexPath, error)) {
        DUST_ERROR("[File] {} doesn't exist (error {} : {})", vertexPath, error.value(), error.message());
        return nullptr;
    }
    if(!fs::exists(fragmentPath, error)) {
        DUST_ERROR("[File] {} doesn't exist (error {} : {})", fragmentPath, error.value(), error.message());
        return nullptr;
    }

    // read files
    return dust::createRef<Shader>(
        dust::io::getFileRawContent(vertexPath),
        dust::io::getFileRawContent(fragmentPath)
    );
}

u32 dr::Shader::getUniformLocation(const std::string &name)
{
    return glGetUniformLocation(m_renderID, name.c_str());
}

u32 dr::Shader::compileShader(int type, const std::string& code)
{
    u32 id = glCreateShader(type);
    const char* codeRaw = code.c_str();
    glShaderSource(id, 1, &codeRaw, nullptr);
    glCompileShader(id);

    int success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(id, INFO_LOG_SIZE, NULL, infoLog);
        DUST_ERROR("[OpenGL][Shader] : {}", infoLog);
    }
    return id;
}
bool dr::Shader::linkShaders(u32 vertexShader, u32 fragmentShader)
{
    glAttachShader(m_renderID, vertexShader);
    glAttachShader(m_renderID, fragmentShader);
    glLinkProgram(m_renderID);

    int success = 0;
    glGetProgramiv(m_renderID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(m_renderID, INFO_LOG_SIZE, NULL, infoLog);
        DUST_ERROR("[OpenGL][Shader Program] : {}", infoLog);
        return false;
    }
    return true;
}