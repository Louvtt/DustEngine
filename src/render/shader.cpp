#include "dust/render/shader.hpp"

#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/io/assetsManager.hpp"
#include "dust/io/loaders.hpp"
#include "dust/render/mesh.hpp"
#include "dust/render/renderAPI.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <fstream>
#include <system_error>

namespace dr = dust::render;

/// Max size of an OpenGL Uniform Name
#define MAX_UNIFORM_NAME_SIZE 128

/// Size of the log info buffer (to query status log)
#define INFO_LOG_SIZE 2048
inline static char infoLog[INFO_LOG_SIZE];

dr::Shader::Shader(const std::string &vertexCode, const std::string &fragmentCode)
: Shader()
{
    m_renderID = internalCreate(vertexCode, fragmentCode);
}
dr::Shader::Shader()
: m_renderID(0), m_uniforms() {}
dr::Shader::~Shader()
{
    DUST_PROFILE;
    glUseProgram(0);
    glDeleteProgram(m_renderID);
}

void dr::Shader::use() {
    DUST_PROFILE_GPU("UseProgram");
    glUseProgram(m_renderID);
}

void dr::Shader::setUniform(const std::string &name, bool value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform1i bool");
    glProgramUniform1i(m_renderID, loc, (int)value);
}

void dr::Shader::setUniform(const std::string &name, int value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform1i");
    glProgramUniform1i(m_renderID, loc, value);
}

void dr::Shader::setUniform(const std::string &name, float value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform1f");
    glProgramUniform1f(m_renderID, loc, value);
}

void dr::Shader::setUniform(const std::string &name, glm::vec2 value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform2f");
    glProgramUniform2f(m_renderID, loc, value.x, value.y);
}

void dr::Shader::setUniform(const std::string &name, glm::vec3 value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform3f");
    glProgramUniform3f(m_renderID, loc, value.x, value.y, value.z);
}

void dr::Shader::setUniform(const std::string &name, glm::vec4 value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniform4f");
    glProgramUniform4f(m_renderID, loc, value.x, value.y, value.z, value.w);
}
void dr::Shader::setUniform(const std::string &name, glm::mat4 value)
{
    const u32 loc = getUniformLocation(name);
    DUST_PROFILE_GPU("glProgramUniformMatrix4fv");
    glProgramUniformMatrix4fv(m_renderID, loc, 1, GL_FALSE, glm::value_ptr(value));
}

void dr::Shader::reload() {
    DUST_PROFILE_SECTION("Shader::reload");
    const auto &resultVert = dust::io::LoadFile(m_vertexFilePath);
    const auto &resultFrag = dust::io::LoadFile(m_fragmentFilePath);
    if(resultVert.has_value() && resultFrag.has_value()) {
        u32 reloadedShaderID = internalCreate(resultVert.value(), resultFrag.value());
        if(reloadedShaderID != 0) {
            glUseProgram(0);
            glDeleteProgram(m_renderID);
            m_renderID = reloadedShaderID;
        }
    }
}

dust::Result<dr::ShaderPtr>
dr::Shader::loadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
{
    DUST_PROFILE_SECTION("Shader::loadFromFile");
    // read files
    const auto &resultVert = dust::io::LoadFile(vertexPath);
    const auto &resultFrag = dust::io::LoadFile(fragmentPath);
    if(resultVert.has_value() && resultFrag.has_value()) {
        auto res = dust::createRef<Shader>(
            resultVert.value(), resultFrag.value()
        );
        res->m_vertexFilePath   = vertexPath;
        res->m_fragmentFilePath = fragmentPath;
        return res;
    }
    return {};
}

u32 dr::Shader::getUniformLocation(const std::string &name)
{
    DUST_PROFILE;
    const auto found = m_uniforms.find(name);
    if(found == m_uniforms.end()) {
        return -1;
    }
    return found->second.index;
}

u32 dr::Shader::internalCreate(const std::string &vertexCode, const std::string &fragmentCode)
{
    DUST_PROFILE_GPU("Shader program creation");
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
    if(renderID == 0) { 
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return m_renderID; 
    }
    if(!linkShaders(renderID, vertex, fragment)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(renderID);
        return m_renderID;
    }
    // validate
    if(!validateProgram(renderID)) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(renderID);
        return m_renderID;
    }

    // Parse active uniforms
    queryActiveUniforms(renderID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    DUST_DEBUG("[OpenGL] Created Shader {}", renderID);
    return renderID;
}

u32 dr::Shader::compileShader(int type, const std::string& code)
{
    DUST_PROFILE_GPU("Shader internal create");
    const u32 id = glCreateShader(type);
    if(id == 0) return id; // ERROR
    const char* codeRaw = code.c_str();
    glShaderSource(id, 1, &codeRaw, nullptr);
    glCompileShader(id);

    int success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(id, INFO_LOG_SIZE, NULL, infoLog);
        glDeleteShader(id);
        DUST_ERROR("[OpenGL][Shader Compilation] : {}", infoLog);
        return id;
    }
    return id;
}
bool dr::Shader::linkShaders(u32 program, u32 vertexShader, u32 fragmentShader)
{
    DUST_PROFILE_GPU("Shader internal link");
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

bool dr::Shader::validateProgram(u32 program)
{
    glValidateProgram(program);
    int success = 0;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, INFO_LOG_SIZE, NULL, infoLog);
        DUST_ERROR("[OpenGL][Shader Validation] : {}", infoLog);
        return false;
    }
    return true;
}

void dr::Shader::queryActiveUniforms(u32 program)
{
    DUST_PROFILE_SECTION("Shader::queryActiveUniforms");
    DUST_PROFILE_GPU("glGetActiveUniform queries");
    m_uniforms.clear(); // empty uniforms.
    int uniformCount;
    glUseProgram(program);
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);
    u32 type;
    int length, size;
    char name[MAX_UNIFORM_NAME_SIZE];
    for(u32 i = 0; i < uniformCount; ++i) {
        glGetActiveUniform(program, i, MAX_UNIFORM_NAME_SIZE, &length, &size, &type, name);
        u32 uIndex = glGetUniformLocation(program, name);
        dr::Uniform u{name, UniformType::Float, uIndex};
        switch(type) {
            case GL_FLOAT:      u.type = UniformType::Float; break;
            case GL_FLOAT_VEC2: u.type = UniformType::Float2; break;
            case GL_FLOAT_VEC3: u.type = UniformType::Float3; break;
            case GL_FLOAT_VEC4: u.type = UniformType::Float4; break;
            case GL_INT:        u.type = UniformType::Int; break;
            case GL_BOOL:       u.type = UniformType::Bool; break;
            case GL_FLOAT_MAT4: u.type = UniformType::Mat4; break;

            case GL_SAMPLER_2D:   u.type = UniformType::Sampler2D; break;
            case GL_SAMPLER_3D:   u.type = UniformType::Sampler3D; break;
            case GL_SAMPLER_CUBE: u.type = UniformType::SamplerCube; break;
        }
        DUST_DEBUG("[OpenGL][Shader Uniform] {} at {} (type = {})", name, uIndex, type);
        m_uniforms.insert({
            std::string(name), u 
        });
    }
    glUseProgram(0);
}

