#ifndef _DUST_RENDER_SHADER_HPP_
#define _DUST_RENDER_SHADER_HPP_

#include "dust/core/types.hpp"
#include "dust/io/resourceFile.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <tuple>
#include <unordered_map>

#ifdef EMSCRIPTEN
#define SHADER_PREFIX "precision highp float"
#else
#define SHADER_PREFIX ""
#endif

#define SHADER_SOURCE(shader_version, ...) shader_version "\n" #__VA_ARGS__

namespace dust::render {
/**
 * @brief Type of the uniform in a shader program
 */
enum class UniformType {
    /// Floating types
    Float, Float2, Float3, Float4,
    /// Integer types
    Int,
    /// Boolean
    Bool,
    /// Matrix types
    Mat4,
    /// Textures Samplers
    Sampler2D, Sampler3D, SamplerCube,
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
class Shader : public io::ResourceFile
{
protected:
    /// OpenGL Shader program ID
    u32 m_renderID{0};

    /// File path to the vertex Shader file
    std::string m_vertexFilePath{};
    /// File path to the fragment Shader file
    std::string m_fragmentFilePath{};

    /// Map of the Uniforms location in the Shader
    std::unordered_map<std::string, Uniform> m_uniforms;
public:
    /**
     * @brief Create a new Shader Program (With vertex and fragment)
     *
     * @param vertexCode vertex shader code
     * @param fragmentCode fragment shader code
     */
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    /**
     * @brief Destroy the Shader Program
     */
    virtual ~Shader();

    /**
     * @brief Use this Shader Program
     */
    void use() const;

    /**
     * @brief Set an int Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, int value);
    /**
     * @brief Set a bool Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, bool value);
    /**
     * @brief Set a float Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, float value);
    /**
     * @brief Set a vec2 Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, glm::vec2 value);
    /**
     * @brief Set a vec3 Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, glm::vec3 value);
    /**
     * @brief Set a vec4 Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, glm::vec4 value);
    /**
     * @brief Set a mat4 Uniform
     *
     * @param name uniform name in the shader
     * @param value value of this uniform
     */
    void setUniform(const std::string &name, glm::mat4 value);

    /**
     * @brief Reload the Shader Program from previous filePath(s)
     * 
     * In case it has an error on loading, it will not refresh the attachment 
     */
    void reload(bool firstLoad) override;
    
    /**
     * @brief Load a shader from vertex and fragment files
     *
     * @param vertexPath vertex file path 
     * @param fragmentPath fragment file path 
     * @return Result<Ref<Shader>> The loaded shader from path (if found)
     */
    static Result<Ref<Shader>> LoadFromFile(const std::string &vertexPath, const std::string &fragmentPath);

protected:
    /**
     * @brief Construct a new empty Shader Program
     */
    Shader();

protected:
    /**
     * @brief Internal OpenGL create function 
     * @param vertexCode the vertex shader code
     * @param fragmentCode the fragment shader code
     * @return u32 the OpenGL shader program ID
     */
    u32 internalCreate(const std::string &vertexCode, const std::string &fragmentCode);

    /**
     * @brief Get the Uniform Location in the m_uniforms map
     * @param name Name of the uniform
     * @return u32 the location in the actives uniforms
     * @return -1 if not found
     */
    u32 getUniformLocation(const std::string &name);
    /**
     * @brief Compile a shader
     * @param type the OpenGL shader type (i.e. `GL_FRAGMENT_SHADER`)
     * @param code the code of the shader
     * @return u32 the OpenGL shader ID
     */
    u32 compileShader(int type, const std::string& code);
    /**
     * @brief Link the shaders to the shader program
     * @param program the OpenGL shader program ID
     * @param vertexShader the OpenGL vertex shader id
     * @param fragmentShader the OpenGL fragment shader id
     * @return true the shaders was successfully linked
     * @return false the shaders link failed
     */
    bool linkShaders(u32 program, u32 vertexShader, u32 fragmentShader);
    /**
     * @brief Validate the content of a Shader Program
     * @param program the OpenGL shader program
     * @return true the shader program was successfully validated
     * @return false the shader program validation failed
     */
    bool validateProgram(u32 program);

    /**
     * @brief Parse actives uniforms into a map to avoid
     * calling `glGetUniformLocation` each time we want to set an uniform.
     * @param program the OpenGL shader program ID
     */
    void queryActiveUniforms(u32 program);
};
using ShaderPtr = Ref<Shader>;
using ShaderUPtr = Scope<Shader>;


/**********************************************************/

/**
 * @brief Packed Shader Program (onefile shader)
 */
class PackedShader : public Shader {
private:
    std::string m_filePath;
    PackedShader();
public:
    /**
     * @brief Construct a new Packed Shader Program
     * @param code the shader code (see PackedShader::LoadFromFile)
     */
    PackedShader(const std::string &code);
    /**
     * @brief Destroy the Packed Shader Program
     */
    ~PackedShader() = default;

    /**
     * @brief Load a shader from one file, fragment and shader must be declared with 
     * `#pragma shader <type>` with `type` either "fragment" or "vertex".
     * 
     * @param path path of the file
     * @return Result<Ref<Shader>> The loaded shader from path (if found)
     */
     static Result<Ref<PackedShader>> LoadFromFile(const std::string &path);

    /**
     * @brief Reload the Shader Program from previous filePath(s)
     * 
     * In case it has an error on loading, it will not refresh the attachment 
     */
    void reload(bool firstLoad) override;

private:
    std::tuple<std::string, std::string> processCode(const std::string &code);
};

}

#endif //_DUST_RENDER_SHADER_HPP_