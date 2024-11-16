#include "dust/dust.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"

#include "dust/editor/editor_scene_view.hpp"
#include "dust/editor/imgui_extensions.hpp"
#include "dust/editor/model_tool.hpp"
#include "dust/render/skybox.hpp"

#include "general_inspector.hpp"

#include <cstdlib>
#include <format>

using namespace dust;

constexpr f32 CAMERA_SPEED        = 100.f;
constexpr f32 CAMERA_ROTATE_SPEED = 50.f;

////////////////////////////////////////////////

class SponzaApp : public Application {
private:
    render::ShaderPtr m_shader;
    render::ShaderPtr m_depthShader;
    render::ShaderPtr m_currentShader;

    Result<render::ModelPtr> m_sponza;
    render::Camera3DPtr m_camera;
    render::SkyboxPtr m_skybox;

    render::DirectionnalLight m_sun;

    render::RenderPassPtr m_simplePass;
    render::PostProcessPassPtr m_postprocessPass;

    bool m_wireframe;
    bool m_drawSponza;
    float m_exposure;

    friend class GeneralInspector;

public:
    explicit SponzaApp()
        : Application("Sponza Demo", 1920u, 1080u), m_shader(nullptr), m_sponza(),
          m_camera(createRef<render::Camera3D>(getWindow()->getWidth(), getWindow()->getHeight(),
                                               90, 2000)),
          m_sun(glm::normalize(glm::vec3(-2.0f, 4.0f, -1.0f)), {1.f, 1.f, 1.f}),
          m_drawSponza(true), m_wireframe(false), m_simplePass(nullptr),
          m_postprocessPass(nullptr), m_exposure(1.)
    {
        getWindow()->setVSync(false);

        const auto shader= render::PackedShader::LoadFromFile("assets/pbr.glsl");
        if (!shader.has_value()) {
            DUST_ERROR("Exiting ... PBR Shader missing");
            exit(EXIT_FAILURE);
        }
        m_shader = shader.value();

        const auto depthShader =
            render::Shader::LoadFromFile("assets/shader.vert", "assets/depth.frag");
        if (!depthShader.has_value()) {
            DUST_ERROR("Exiting ... Depth Shader missing");
            exit(EXIT_FAILURE);
        }
        m_depthShader   = depthShader.value();
        m_currentShader = m_shader;

        m_sponza = io::LoadModel("assets/sponza_gltf/sponza.gltf");
        render::PBRMaterial::SetupMaterialShader(m_shader.get());

        m_camera->setPosition(glm::vec3(0.f, 20.f, 0.f));

        m_skybox = render::SkyboxPtr(new render::Skybox({
            "assets/cubemap/right.png",
            "assets/cubemap/left.png",
            "assets/cubemap/top.png",
            "assets/cubemap/bottom.png",
            "assets/cubemap/front.png",
            "assets/cubemap/back.png",
        }));

        m_simplePass = createRef<render::RenderPass>(render::RenderPassDesc{
            m_currentShader, createRef<render::Framebuffer>(render::Framebuffer::Desc{
                {{render::Framebuffer::AttachmentType::COLOR_HDR, true},
                {render::Framebuffer::AttachmentType::DEPTH_STENCIL, false}},
                getWindow()->getWidth(),
                getWindow()->getHeight()}
            )
        });

        updateUniforms();

        // sky color until skybox is created
        // getRenderer()->setClearColor(0/255.f, 179/255.f, 255/255.f);

        auto resultBuffer = m_simplePass->getFramebuffer();
        getEditor()->add_tool(new EditorSceneView(resultBuffer));
        auto modelTool = new ModelTool();
        modelTool->set_inspected_model(m_sponza.value().get());
        getEditor()->add_tool(modelTool);
        getEditor()->add_tool(new GeneralInspector());

        DUST_INFO("== Example Sponza loaded! ==");
    }

    ~SponzaApp() {
        m_shader.reset();
        m_depthShader.reset();

        m_simplePass.reset();
        m_postprocessPass.reset();
        m_skybox.reset();

        m_sponza.reset();

        m_camera.reset();
    }

    void update() override {
        dust::Application::update();
        f32 delta = (f32)getTime().delta;
        if (InputManager::IsKeyDown(Key::A)) {
            m_camera->rotate({-CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::D)) {
            m_camera->rotate({CAMERA_ROTATE_SPEED * delta, .0f, .0f});
            m_camera->bind(m_currentShader.get());
        }

        if (InputManager::IsKeyDown(Key::W)) {
            m_camera->move(m_camera->forward() * -CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::S)) {
            m_camera->move(m_camera->forward() * CAMERA_SPEED * delta);
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::Space)) {
            m_camera->move(glm::vec3(0, CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
        if (InputManager::IsKeyDown(Key::LeftShift)) {
            m_camera->move(glm::vec3(0, -CAMERA_SPEED * delta, 0));
            m_camera->bind(m_currentShader.get());
        }
    }

    void render() override {
        dust::Application::render();

        // Main Rendering
        {
            DUST_PROFILE_SECTION("Scene Render");

            // Scene incrustation inside editor
            const auto scene_view = getEditor()->get_tool("Scene");
            if(scene_view.has_value()) {
                auto editor_scene_view = (EditorSceneView *)scene_view.value();
                if (editor_scene_view->was_resized()) {
                    auto size = editor_scene_view->get_size();
                    getRenderer()->resize(size.x, size.y);
                    m_simplePass->getFramebuffer()->resize(size.x, size.y);
                    m_camera->resize(size.x, size.y);
                    m_camera->bind(m_currentShader.get()); // update
                }
            }

            m_simplePass->preRender();
            {
                DUST_PROFILE_GPU("Sponza render");
                getRenderer()->clear();
                // sponza
                if (m_drawSponza && m_sponza.has_value()) {
                    m_sponza.value()->draw(m_currentShader.get());
                }

                // skybox
                m_skybox->draw(m_camera.get());
            }
            m_simplePass->postRender();
        }
    }

private:
    void updateUniforms() {
        m_camera->bind(m_currentShader.get());
        m_sun.bind(m_currentShader);
        m_currentShader->setUniform("uLightCount", 1);
        m_currentShader->setUniform("uExposure", m_exposure);
    }
};

DUST_SIMPLE_ENTRY(SponzaApp)

///////////////////////////////////////////////////

GeneralInspector::GeneralInspector() : EditorTool("Inspector") {}

bool GeneralInspector::is_panel_tool() const { return true; }

void GeneralInspector::render_ui() {
    auto a = (SponzaApp*)Application::Get();
    DUST_PROFILE_SECTION("ImGui Editor");
    {
        ImGui::Text("FPS: %d", (u32)(1. / a->getTime().delta));

        ImGui::SeparatorText("Camera");
        {
            ImGui::InputMat4("Projection", a->m_camera->getProj());
            ImGui::InputMat4("View", a->m_camera->getView());
        }

        ImGui::SeparatorText("Lighting");
        {

            ImGui::SliderFloat("Exposure", &a->m_exposure, .1, 5., "%.2f");

            glm::vec3 sunDir = a->m_sun.getDirection();
            if (ImGui::InputFloat3("Sun Direction", glm::value_ptr(sunDir), "%.3f",
                                   ImGuiInputTextFlags_EnterReturnsTrue)) {
                sunDir = glm::normalize(sunDir);
                a->m_sun.setDirection(sunDir);
                a->updateUniforms();
            }
            glm::vec3 sunColor = a->m_sun.getColor();
            if (ImGui::ColorEdit3("Sun color", glm::value_ptr(sunColor))) {
                a->m_sun.setColor(sunColor);
                a->updateUniforms();
            }
        }

        ImGui::SeparatorText("Shaders");
        {
            if (ImGui::Checkbox("Wireframe", &a->m_wireframe)) {
                a->getRenderer()->setDrawWireframe(a->m_wireframe);
            }
            ImGui::Checkbox("Draw Sponza", &a->m_drawSponza);

            if (ImGui::Button("Show Depth")) {
                a->m_currentShader = a->m_depthShader;
            }
            if (ImGui::Button("Show Render")) {
                a->m_currentShader = a->m_shader;
            }

            // reload shaders ?
            if (ImGui::Button("Reload Shaders")) {
                a->m_shader->reload(false);
                a->m_depthShader->reload(false);
                render::PBRMaterial::SetupMaterialShader(a->m_shader.get());
                a->updateUniforms();
            }
        }
    }
}
