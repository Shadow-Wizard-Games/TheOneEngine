#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "UniformBuffer.h"
#include "EngineCore.h"
#include "GameObject.h"
#include "N_SceneManager.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "Ray.h"


static RendererSettings settings;
static RendererData renderer;


void Renderer::Init()
{
	Renderer2D::Init();
	Renderer3D::Init();

	renderer.cameraUniformBuffer = std::make_shared<UniformBuffer>(sizeof(glm::mat4), 0);
}

void Renderer::Update()
{
    Renderer2D::ResetStats();
    Renderer3D::ResetStats();
	for (auto& target : renderer.renderTargets)
	{
        if (!target.GetCamera() || !target.IsActive())
            continue;

        DrawMode drawMode = target.GetMode();

        Renderer::SetRenderEnvironment();
        SetUniformBufferCamera(target.GetCamera()->viewProjectionMatrix);
        engine->N_sceneManager->currentScene->Draw(drawMode, target.GetCamera());
		Renderer3D::Update(target);

        // gBuffer ------------------------------------------------
        Renderer3D::GeometryPass(target);

        if (settings.particlesLight.isEnabled)
            Renderer2D::Update(BatchType::WORLD, target);

        // shadowBuffer -------------------------------------------
        if (drawMode != DrawMode::BUILD_DEBUG)
        {
            if (settings.shadows.isEnabled)
                Renderer3D::ShadowPass(target);
        }

        // postBuffer ---------------------------------------------
        // Blit gBuffer Depth to postBuffer
        if (drawMode != DrawMode::BUILD_DEBUG)
        {
            if (Renderer3D::InitPostProcess(target))
            {
                if (settings.lights.isEnabled)
                    Renderer3D::LightPass(target);

                Renderer3D::IndexPass(target);

                if (Renderer::Settings()->particles.isEnabled)
                    Renderer2D::Update(BatchType::WORLD, target);
            }
            Renderer3D::EndPostProcess(target);

            if (drawMode == DrawMode::EDITOR)
            {
                Renderer::DrawDebug();
                Renderer2D::Update(BatchType::EDITOR, target);
            }
        }       

        // uiBuffer -----------------------------------------------
        // Blit postBuffer Color to uiBuffer, draw UI on top
        if (drawMode != DrawMode::EDITOR)
        {
            Renderer3D::UIComposition(target);

            if (settings.crt.isEnabled)
                Renderer3D::CRTShader(target);
        }       

        // BUILD --------------------------------------------------
        // Blit final composition to 0 Buffer
        if (drawMode == DrawMode::BUILD_DEBUG || drawMode == DrawMode::BUILD_RELEASE)
        {
            FrameBuffer* uibuffer = target.GetFrameBuffer("uiBuffer");
            int glColorAttachmentID = drawMode == DrawMode::BUILD_RELEASE ? GL_COLOR_ATTACHMENT1 : GL_COLOR_ATTACHMENT0;

            GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, uibuffer->GetBuffer()));
            GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            GLCALL(glReadBuffer(glColorAttachmentID));
            GLCALL(glBlitFramebuffer(
                0, 0, uibuffer->GetWidth(), uibuffer->GetHeight(),
                0, 0, uibuffer->GetWidth(), uibuffer->GetHeight(),
                GL_COLOR_BUFFER_BIT, GL_NEAREST));
        }
	}

    Renderer2D::ResetBatches();
    Renderer3D::ResetCalls();
}

void Renderer::Shutdown()
{
	Renderer2D::Shutdown();
	Renderer3D::Shutdown();
    renderer.cameraUniformBuffer->Delete();
}

unsigned int Renderer::AddRenderTarget(std::string name, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers, bool active)
{
	unsigned int id = renderer.renderTargets.size();
	renderer.renderTargets.emplace_back(id, name, mode, camera, viewportSize, frameBuffers, active);
	return id;
}

std::vector<RenderTarget>* Renderer::GetRenderTargets()
{
    return &renderer.renderTargets;
}

RenderTarget* Renderer::GetRenderTarget(unsigned int targetID)
{
    for (auto& target : renderer.renderTargets)
    {
        if (target.GetId() == targetID)
            return &target;
    }
    return nullptr;
}

std::vector<FrameBuffer>* Renderer::GetFrameBuffers(unsigned int targetID)
{
	return renderer.renderTargets[targetID].GetFrameBuffers();
}

FrameBuffer* Renderer::GetFrameBuffer(unsigned int targetID, std::string name)
{
	return renderer.renderTargets[targetID].GetFrameBuffer(name);
}

void Renderer::SetUniformBufferCamera(const glm::mat4& camMatrix)
{
	renderer.cameraUniformBuffer->SetData(&camMatrix, sizeof(glm::mat4));
}

void Renderer::SetRenderEnvironment()
{
    GLCALL(glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthFunc(GL_LEQUAL));
    GLCALL(glEnable(GL_CULL_FACE));
    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCALL(glEnable(GL_COLOR_MATERIAL));
}

void Renderer::AddRay(Ray ray)
{
    renderer.rays.push_back(ray);
}

void Renderer::ClearRays()
{
    renderer.rays.clear();
}


//@Get / Set ----------------------------------------------------------------
RendererSettings* Renderer::Settings()
{
    return &settings;
}


//@Draw Utils ----------------------------------------------------------------
// Utils
void Renderer::DrawScreenQuad()
{
    // Disable writing to the depthbuffer, 
    // otherwise DrawScreenQuad overwrites it
    GLCALL(glDepthMask(GL_FALSE));

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
    glBindVertexArray(0);
    GLCALL(glDepthMask(GL_TRUE));
}

void Renderer::DrawDebug(bool override)
{
    if (settings.axis.isEnabled || override)
        DrawAxis();

    if (settings.grid.isEnabled || override)
        DrawGrid(5000, 50);

    if (settings.collisions.isEnabled || override)
        engine->collisionSolver->DrawCollisions();

    if (settings.scriptShapes.isEnabled || override)
        engine->monoManager->RenderShapesQueue();

    if (auto selectedGO = engine->N_sceneManager->GetSelectedGO())
    {
        Camera* gameCam = selectedGO.get()->GetComponent<Camera>();

        if (gameCam != nullptr && gameCam->drawFrustum)
            DrawFrustum(gameCam->frustum);
    }

    if (settings.raycasting.isEnabled || override)
    {
        for (auto &ray : renderer.rays)
            DrawRay(ray);
    }
}

void Renderer::DrawAxis()
{
    glm::vec3 origin = { 0.0f, 0.0f, 0.0f };
    glm::vec3 xaxis = { 0.8f, 0.0f, 0.0f };
    glm::vec3 yaxis = { 0.0f, 0.8f, 0.0f };
    glm::vec3 zaxis = { 0.0f, 0.0f, 0.8f };

    Renderer2D::DrawLine(BatchType::EDITOR, origin, xaxis, { 1.0f, 0.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, origin, yaxis, { 0.0f, 1.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, origin, zaxis, { 0.0f, 0.0f, 1.0f, 1.0f });
}

void Renderer::DrawGrid(int grid_size, int grid_step)
{
    // Define maximum distance for fading
    float maxDistance = grid_size * 2; // You may adjust this value as needed

    for (int i = -grid_size; i <= grid_size; i += grid_step)
    {
        // Calculate distance from camera
        float distance = sqrt(pow(i, 2) + pow(0.0f, 2) + pow(-grid_size, 2)); // Example camera position: (0, 0, -grid_size)

        // Calculate alpha value based on distance
        float alpha = 1.0f - (distance / maxDistance); // Linear fade, adjust as needed

        // Clamp alpha value between 0 and 1
        alpha = CLAMP(alpha, 1.0f, 0.0f);

        glm::vec3 v1 = { i, 0.0f, -grid_size };
        glm::vec3 v2 = { i, 0.0f,  grid_size };
        glm::vec3 v3 = { -grid_size, 0.0f, i };
        glm::vec3 v4 = { grid_size, 0.0f, i };
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, alpha };

        Renderer2D::DrawLine(BatchType::EDITOR, v1, v2, color);
        Renderer2D::DrawLine(BatchType::EDITOR, v3, v4, color);
    }
}

void Renderer::DrawFrustum(const Frustum& frustum)
{
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[0], frustum.vertices[1], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[1], frustum.vertices[2], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[2], frustum.vertices[3], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[3], frustum.vertices[0], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[4], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[5], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[6], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[7], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[0], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[1], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[2], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BatchType::EDITOR, frustum.vertices[3], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
}

void Renderer::DrawRay(const Ray& ray)
{
    // Define vertex data
    GLfloat rayVertices[] = {
        ray.Origin.x, ray.Origin.y, ray.Origin.z,
        ray.Origin.x + ray.Direction.x * 1000,
        ray.Origin.y + ray.Direction.y * 1000,
        ray.Origin.z + ray.Direction.z * 1000
    };
    Renderer2D::DrawLine(BatchType::EDITOR, { ray.Origin.x,  ray.Origin.y, ray.Origin.z },
        { ray.Origin.x + ray.Direction.x * 1000,
         ray.Origin.y + ray.Direction.y * 1000,
         ray.Origin.z + ray.Direction.z * 1000 }, { 1.0f, 1.0f, 1.0f, 1.0f });
}