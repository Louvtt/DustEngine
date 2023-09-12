#include "dust/render/renderer.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/render/renderAPI.hpp"

#include "GLFW/glfw3.h"
#include "backends/imgui_impl_opengl3.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

#pragma region "OpenGL Utils"

#define GLCASETOSTR(name)                                                      \
    case name:                                                                 \
        return #name;

const char *getGLSourceStr(GLenum _source) {
    switch (_source) {
        GLCASETOSTR(GL_DEBUG_SOURCE_API)
        GLCASETOSTR(GL_DEBUG_SOURCE_APPLICATION)
        GLCASETOSTR(GL_DEBUG_SOURCE_OTHER)
        GLCASETOSTR(GL_DEBUG_SOURCE_SHADER_COMPILER)
        GLCASETOSTR(GL_DEBUG_SOURCE_THIRD_PARTY)
        GLCASETOSTR(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
    }
    return "GL_DEBUG_SOURCE_UNKNOWN";
}

const char *getGLTypeStr(GLenum _type) {
    switch (_type) {
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

const char *getGLSeverityStr(GLenum _severity) {
    switch (_severity) {
        GLCASETOSTR(GL_DEBUG_SEVERITY_LOW)
        GLCASETOSTR(GL_DEBUG_SEVERITY_MEDIUM)
        GLCASETOSTR(GL_DEBUG_SEVERITY_HIGH)
        GLCASETOSTR(GL_DEBUG_SEVERITY_NOTIFICATION)
        GLCASETOSTR(GL_DONT_CARE)
    }
    return "GL_SEVERITY_UNKNOWN";
}

const char *getGLIDStr(GLuint _id) {
    switch (_id) {
        GLCASETOSTR(GL_INVALID_ENUM)
        GLCASETOSTR(GL_INVALID_FRAMEBUFFER_OPERATION)
        GLCASETOSTR(GL_INVALID_INDEX)
        GLCASETOSTR(GL_INVALID_OPERATION)
        GLCASETOSTR(GL_INVALID_VALUE)
    }
    return "GL_UNKNOWN";
}

#undef GLCASETOSTR

static void glDebugCallback(GLenum _source, GLenum _type, GLuint _id,
                            GLenum _severity, GLsizei _length,
                            const GLchar *_message, const void *_userParam) {
    if (_severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        DUST_INFO("[OpenGL] [{} | {}]({}): {}", getGLTypeStr(_type),
                  getGLSourceStr(_source), getGLIDStr(_id), _message);
        return;
    }

    DUST_ERROR("[OpenGL] [{} | {} | {}]({}): {}", getGLSeverityStr(_severity),
               getGLTypeStr(_type), getGLSourceStr(_source), getGLIDStr(_id),
               _message);
}
#pragma endregion

dust::Renderer::Renderer(const dust::Window &window) {
    DUST_PROFILE_SECTION("Renderer::Constructor");
    // init glad
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        DUST_ERROR("[Glad] Failed to load OpenGL");
        return;
    }
    m_initialized = true;
    m_renderApiName = (const char *)glGetString(GL_RENDERER);
    m_renderApiVersion = (const char *)glGetString(GL_VERSION);
    DUST_INFO("[OpenGL] Loaded OpenGL {} using {}", m_renderApiVersion,
              m_renderApiName);
    DUST_PROFILE_GPU_SETUP;

    // avoid skipping pixels
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _DEBUG
    DUST_INFO("[OpengL] Set up debug message callback.");
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugCallback, nullptr);

#endif
    if (!GLAD_GL_ARB_geometry_shader4)
        DUST_WARN("Nsight will output errors. (GL_ARB_geometry_shader4 not "
                  "supported)");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    setClearColor(.1f, .1f, .1f);
    resize(window.getWidth(), window.getHeight());

    // Query device driver informations

    // const char* shading_version = (const
    // char*)glGetString(GL_SHADING_LANGUAGE_VERSION); DUST_DEBUG("[OpenGL]
    // Shading Language version : {}", shading_version);
    if (!ImGui_ImplOpenGL3_Init("#version 460 core")) {
        DUST_ERROR("[OpenGL][ImGui] Failed to load ImGui for OpenGL.");
    } else {
        DUST_INFO("[OpenGL][ImGui] Loaded ImGui for OpenGL.");
    }
}

dust::Renderer::~Renderer() {
    DUST_PROFILE;
    ImGui_ImplOpenGL3_Shutdown();
    DUST_INFO("[Glad] Unloading OpenGL");
}

void dust::Renderer::newFrame() {
    DUST_PROFILE_GPU("renderer new frame");
    clear();
    ImGui_ImplOpenGL3_NewFrame();
}

void dust::Renderer::clear(bool clearColor) {
    DUST_PROFILE_GPU("renderer clear");
    int clearBits = GL_STENCIL_BUFFER_BIT;

    if (m_depthEnabled) {
        clearBits |= GL_DEPTH_BUFFER_BIT;
    }

    if (clearColor) {
        clearBits |= GL_COLOR_BUFFER_BIT;
    }

    glClear(clearBits);
}

void dust::Renderer::endFrame() {
    DUST_PROFILE_GPU("end frame");
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dust::Renderer::setCulling(bool culling) {
    DUST_PROFILE_GPU("renderer set culling");
    if (culling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
void dust::Renderer::setCullFaces(bool back, bool front) {
    DUST_PROFILE_GPU("renderer set cull face");
    glCullFace(back ? (front ? GL_FRONT_AND_BACK : GL_BACK)
                    : (front ? GL_FRONT : GL_BACK));
}

void dust::Renderer::setClearColor(float r, float g, float b, float a) {
    DUST_PROFILE_GPU("renderer set clear color");
    glClearColor(r, g, b, a);
}
void dust::Renderer::setClearColor(glm::vec4 color) {
    setClearColor(color.x, color.y, color.z, color.w);
}

void dust::Renderer::setDepthWrite(bool write) {
    DUST_PROFILE_GPU("renderer set depth write");
    glDepthMask(write);
}
void dust::Renderer::setDepthTest(bool test) {
    DUST_PROFILE_GPU("renderer set depth test");
    if (test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void dust::Renderer::resize(u32 width, u32 height) {
    DUST_PROFILE_GPU("renderer resize");
    glViewport(0, 0, width, height);
}

void dust::Renderer::setDrawWireframe(bool wireframe) {
    DUST_PROFILE_GPU("renderer set wireframe");
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
