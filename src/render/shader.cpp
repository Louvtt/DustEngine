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

#define INFO_LOG_SIZE 2048
static char infoLog[INFO_LOG_SIZE];

dr::Shader::Shader(const std::string &vertexCode, const std::string &fragmentCode)
: Shader()
{
    m_renderID = internalCreate(vertexCode, fragmentCode);
}
dr::Shader::Shader()
: m_renderID(0) {}
dr::Shader::~Shader()
{
    glDeleteProgram(m_renderID);
}

void dr::Shader::use() {
    glUseProgram(m_renderID);
}


void dr::Shader::setUniform(const std::string &name, int value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform1i(loc, value);
    }
}

void dr::Shader::setUniform(const std::string &name, float value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform1f(loc, value);
    }
}

void dr::Shader::setUniform(const std::string &name, glm::vec2 value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform2f(loc, value.x, value.y);
    }
}

void dr::Shader::setUniform(const std::string &name, glm::vec3 value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform3f(loc, value.x, value.y, value.z);
    }
}

void dr::Shader::setUniform(const std::string &name, glm::vec4 value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniform4f(loc, value.x, value.y, value.z, value.w);
    }
}
void dr::Shader::setUniform(const std::string &name, glm::mat4 value)
{
    const u32 loc = getUniformLocation(name);
    if(loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void dr::Shader::reload() {
    u32 reloadedShaderID = internalCreate(
        dust::io::getFileRawContent(m_vertexFilePath),
        dust::io::getFileRawContent(m_fragmentFilePath)
    );
    if(reloadedShaderID != 0) {
        glDeleteProgram(m_renderID);
        m_renderID = reloadedShaderID;
    }
}

dr::Shader*
dr::Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::error_code error{};
    if(!fs::exists(vertexPath, error)) {
        DUST_ERROR("[File] {} doesn't exist (error {} : {})", vertexPath, error.value(), error.message());
        return new NullShader();
    }
    if(!fs::exists(fragmentPath, error)) {
        DUST_ERROR("[File] {} doesn't exist (error {} : {})", fragmentPath, error.value(), error.message());
        return new NullShader();
    }

    // read files
    auto res = new Shader(
        dust::io::getFileRawContent(vertexPath),
        dust::io::getFileRawContent(fragmentPath)
    );
    res->m_vertexFilePath   = vertexPath;
    res->m_fragmentFilePath = fragmentPath;
    return res;
}

u32 dr::Shader::getUniformLocation(const std::string &name)
{
    return glGetUniformLocation(m_renderID, name.c_str());
}

u32 dr::Shader::internalCreate(const std::string &vertexCode, const std::string &fragmentCode)
{
    const u32 vertex   = compileShader(GL_VERTEX_SHADER, vertexCode);
    if(vertex == 0) { 
        DUST_ERROR("[OpenGL][Shader] Failed to create a vertex shader."); 
        return m_renderID; 
    }
    const u32 fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if(fragment == 0) { 
        DUST_ERROR("[OpenGL][Shader] Failed to create a fragment shader."); 
        return m_renderID; 
    }
    const u32 renderID = glCreateProgram();
    if(renderID == 0) { return m_renderID; }
    if(!linkShaders(renderID, vertex, fragment)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(renderID);
        return m_renderID;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    DUST_DEBUG("[OpenGL] Created Shader {}", renderID);
    return renderID;
}

u32 dr::Shader::compileShader(int type, const std::string& code)
{
    const u32 id = glCreateShader(type);
    if(id == 0) return id;
    const char* codeRaw = code.c_str();
    glShaderSource(id, 1, &codeRaw, nullptr);
    glCompileShader(id);

    int success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(id, INFO_LOG_SIZE, NULL, infoLog);
        DUST_ERROR("[OpenGL][Shader Compilation] : {}", infoLog);
    }
    return id;
}
bool dr::Shader::linkShaders(u32 program, u32 vertexShader, u32 fragmentShader)
{
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, INFO_LOG_SIZE, NULL, infoLog);
        DUST_ERROR("[OpenGL][Shader Link] : {}", infoLog);
        return false;
    }
    return true;
}

// NULLSHADER

dr::NullShader::NullShader()
: Shader() {}
void dr::NullShader::use() {
    DUST_WARN("[Shader] Using NullShader.");
}
void dr::NullShader::setUniform(const std::string &name, int value) {}
void dr::NullShader::setUniform(const std::string &name, float value) {}
void dr::NullShader::setUniform(const std::string &name, glm::vec2 value) {}
void dr::NullShader::setUniform(const std::string &name, glm::vec3 value) {}
void dr::NullShader::setUniform(const std::string &name, glm::vec4 value) {}
void dr::NullShader::setUniform(const std::string &name, glm::mat4 value) {}
void dr::NullShader::reload() {}

dr::NullShader::operator bool() const {
    return false;
}
