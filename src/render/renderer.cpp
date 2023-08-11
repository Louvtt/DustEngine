#include "dust/render/renderer.hpp"
#include "GLFW/glfw3.h"
#include "dust/core/log.hpp"
#include "dust/render/renderAPI.hpp"
#include <GL/gl.h>

#pragma region "OpenGL Utils"

#define GLCASETOSTR(name) case name: return #name;

const char* getGLSourceStr(GLenum _source)
{
    switch(_source)
    {
        GLCASETOSTR(GL_DEBUG_SOURCE_API)
        GLCASETOSTR(GL_DEBUG_SOURCE_APPLICATION)
        GLCASETOSTR(GL_DEBUG_SOURCE_OTHER)
        GLCASETOSTR(GL_DEBUG_SOURCE_SHADER_COMPILER)
        GLCASETOSTR(GL_DEBUG_SOURCE_THIRD_PARTY)
        GLCASETOSTR(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
    }
    return "GL_DEBUG_SOURCE_UNKNOWN";
}

const char* getGLTypeStr(GLenum _type)
{
    switch(_type)
    {
        GLCASETOSTR(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        GLCASETOSTR(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        GLCASETOSTR(GL_DEBUG_TYPE_ERROR)
        GLCASETOSTR(GL_DEBUG_TYPE_MARKER)
        GLCASETOSTR(GL_DEBUG_TYPE_OTHER)
        GLCASETOSTR(GL_DEBUG_TYPE_PERFORMANCE)
        GLCASETOSTR(GL_DEBUG_TYPE_POP_GROUP)
        GLCASETOSTR(GL_DEBUG_TYPE_PORTABILITY)
        GLCASETOSTR(GL_DEBUG_TYPE_PUSH_GROUP)
    }
    return "GL_DEBUG_TYPE_UNKNOWN";
}

const char* getGLSeverityStr(GLenum _severity) 
{
    switch(_severity)
    {
        GLCASETOSTR(GL_DEBUG_SEVERITY_LOW)
        GLCASETOSTR(GL_DEBUG_SEVERITY_MEDIUM)
        GLCASETOSTR(GL_DEBUG_SEVERITY_HIGH)
        GLCASETOSTR(GL_DEBUG_SEVERITY_NOTIFICATION)
        GLCASETOSTR(GL_DONT_CARE)
    }
    return "GL_SEVERITY_UNKNOWN";
}

const char* getGLIDStr(GLuint _id)
{
    switch(_id)
    {
        GLCASETOSTR(GL_INVALID_ENUM)
        GLCASETOSTR(GL_INVALID_FRAMEBUFFER_OPERATION)
        GLCASETOSTR(GL_INVALID_INDEX)
        GLCASETOSTR(GL_INVALID_OPERATION)
        GLCASETOSTR(GL_INVALID_VALUE)
    }
    return "GL_UNKNOWN";
}

#undef GLCASETOSTR

static void glDebugCallback(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar* _message, const void* _userParam)
{
    if(_severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        DUST_INFO("[OpenGL] [From {}]: {}", getGLSourceStr(_source), _message);
        return;
    }
    
    DUST_ERROR(
        "[OpenGL] [{} | {} | {}]({}): {}",
        getGLSeverityStr(_severity),
        getGLTypeStr(_type),
        getGLSourceStr(_source),
        getGLIDStr(_id),
        _message
    );
}
#pragma endregion


dust::Renderer::Renderer(const dust::Window& window)
{
    // init glad
    if(!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        DUST_ERROR("[Glad] Failed to load OpenGL");
        return;
    }
    m_initialized      = true;
    m_renderApiName    = (const char*)glGetString(GL_RENDERER);
    m_renderApiVersion = (const char*)glGetString(GL_VERSION);
    DUST_INFO("[OpenGL] Loaded OpenGL {}_{}", m_renderApiName, m_renderApiVersion);

    glEnable(GL_STENCIL);
    glEnable(GL_DEPTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA);
    #ifdef _DEBUG
    DUST_INFO("[OpengL] Set up debug message callback.");
    glDebugMessageCallback(glDebugCallback, nullptr);
    #endif

    setClearColor(.1f, .1f, .1f);
    resize(window.getWidth(), window.getHeight());
}
dust::Renderer::~Renderer()
{
    DUST_INFO("[Glad] Unloading OpenGL");
    gladLoaderUnloadGL();
}

void dust::Renderer::newFrame()
{
    if(m_depthEnabled) {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}
void dust::Renderer::endFrame()
{}

void dust::Renderer::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}
void dust::Renderer::setClearColor(glm::vec4 color)
{
    setClearColor(color.x, color.y, color.z, color.w);
}

void dust::Renderer::setDepthWrite(bool write)
{
    glDepthMask(write);
}
void dust::Renderer::setDepthTest(bool test)
{
    if(test) {
        glEnable(GL_DEPTH);
    } else {
        glDisable(GL_DEPTH);
    }
}

void dust::Renderer::resize(u32 width, u32 height)
{
    glViewport(0, 0, width, height);
}