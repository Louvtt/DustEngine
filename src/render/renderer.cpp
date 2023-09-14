#include "dust/render/renderer.hpp"
#include "dust/core/log.hpp"
#include "dust/core/profiling.hpp"
#include "dust/render/renderAPI.hpp"

#include "GLFW/glfw3.h"
#include "backends/imgui_impl_opengl3.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyOpenGL.hpp"

#pragma region "OpenGL Utils"

const char *getGLSourceStr(GLenum _source) {
    switch (_source) {
        case GL_DEBUG_SOURCE_API:             return "[API]";
        case GL_DEBUG_SOURCE_APPLICATION:     return "[Application]";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "[ShaderCompiler]";
        case GL_DEBUG_SOURCE_THIRD_PARTY:     return "[ThirdParty]";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "[WindowSystem]";
        case GL_DEBUG_SOURCE_OTHER:           return "[Other]";
    }
    return "[Unknown]";
}

const char *getGLTypeStr(GLenum _type) {
    switch (_type) {
        case GL_DEBUG_TYPE_ERROR:               return "Error";
        case GL_DEBUG_TYPE_MARKER:              return "Marker";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behavior";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
        case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
        case GL_DEBUG_TYPE_PUSH_GROUP:          return "Push Group";
        case GL_DEBUG_TYPE_POP_GROUP:           return "Pop Group";
        case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
        case GL_DEBUG_TYPE_OTHER:               return "Other";
    }
    return "Unknown";
}

const char *getGLSeverityStr(GLenum _severity) {
    switch (_severity) {
        case GL_DEBUG_SEVERITY_LOW:            return "(Low)";
        case GL_DEBUG_SEVERITY_MEDIUM:         return "(Medium)";
        case GL_DEBUG_SEVERITY_HIGH:           return "(High)";
        case GL_DEBUG_SEVERITY_NOTIFICATION:   return "(Notification)";
    }
    return "(Unknown)";
}

const char *getGLIDStr(GLuint _id) {
    switch (_id) {
        case GL_INVALID_ENUM:                   return "Invalid Enum";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "Invalid Framebuffer Operation";
        case GL_INVALID_INDEX:                  return "Invalid Index";
        case GL_INVALID_OPERATION:              return "Invalid Operation";
        case GL_INVALID_VALUE:                  return "Invalid Value";
    }
    return "Unknown";
}

#define GL_CALLBACK_MESSAGE_FORMAT "[OpenGL]{} received {} with {}: {}"
static void glDebugCallback(GLenum _source, GLenum _type, GLuint _id,
                            GLenum _severity, GLsizei _length,
                            const GLchar *_message, const void *_userParam) {
    // ignore some callbacks (drivers specifics)
    if(_id == 131169 || _id == 131185 || _id == 131218 || _id == 131204) return; 

    switch(_severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION: 
            DUST_DEBUG(GL_CALLBACK_MESSAGE_FORMAT, getGLSourceStr(_source),
                getGLTypeStr(_type),  getGLIDStr(_id), _message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            DUST_INFO(GL_CALLBACK_MESSAGE_FORMAT, getGLSourceStr(_source),
                getGLTypeStr(_type),  getGLIDStr(_id), _message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            DUST_WARN(GL_CALLBACK_MESSAGE_FORMAT, getGLSourceStr(_source),
                getGLTypeStr(_type),  getGLIDStr(_id), _message);
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            DUST_ERROR(GL_CALLBACK_MESSAGE_FORMAT, getGLSourceStr(_source),
                getGLTypeStr(_type),  getGLIDStr(_id), _message);
            break;
    }
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

    DUST_INFO("[OpengL] Set up debug message callback.");
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugCallback, nullptr);
#ifdef _DEBUG
    // show everything
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    if (!GLAD_GL_ARB_geometry_shader4)
        DUST_WARN("Nsight will output errors. (GL_ARB_geometry_shader4 not "
                  "supported)");
#else
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0,
                          nullptr, GL_TRUE);
#endif

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
