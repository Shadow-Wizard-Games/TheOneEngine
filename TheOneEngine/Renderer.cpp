#include "Renderer.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include "UniformBuffer.h"
#include "EngineCore.h"
#include "N_SceneManager.h"
#include "Camera.h"
#include "RenderTarget.h"
#include "Ray.h"

static const glm::mat4 cameraUI = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f);

struct RendererData
{
	std::vector<RenderTarget> renderTargets;
	std::shared_ptr<UniformBuffer> cameraUniformBuffer;

    // Render Settings
    bool renderParticles = true;
    bool renderLights = true;
    bool renderShadows = true;

    // Debug
    bool drawGrid = true;
    bool drawAxis = true;
    bool drawCollisions = true;
    bool drawScriptShapes = true;
    bool drawRaycasting = false;

    std::vector<Ray> rays;
};

static RendererData renderer;

void Renderer::Init()
{
	Renderer2D::Init();
	Renderer3D::Init();

	renderer.cameraUniformBuffer = std::make_shared<UniformBuffer>(sizeof(glm::mat4), 0);
}

void Renderer::Update()
{
	for (auto& target : renderer.renderTargets)
	{
        if (!target.GetCamera() || !target.IsActive())
            continue;

        Renderer::SetRenderEnvironment();
        SetUniformBufferCamera(target.GetCamera()->viewProjectionMatrix);
        engine->N_sceneManager->currentScene->Draw(target.GetMode(), target.GetCamera());
		Renderer3D::Update(target);

        Renderer3D::GeometryPass(target);

        if (renderer.renderParticles)
            Renderer2D::Update(BT::WORLD, target);

        // index buffer -> geometry pass of objects with effects

        if (renderer.renderShadows)
            Renderer3D::ShadowPass(target);

        if (renderer.renderLights)
            Renderer3D::PostProcess(target);

        SetUniformBufferCamera(cameraUI);
        Renderer2D::Update(BT::UI, target);
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

unsigned int Renderer::AddRenderTarget(std::string name, DrawMode mode, Camera* camera, glm::vec2 viewportSize, std::vector<std::vector<Attachment>> frameBuffers)
{
	unsigned int id = renderer.renderTargets.size();
	renderer.renderTargets.emplace_back(id, name, mode, camera, viewportSize, frameBuffers);
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


//@Get/Set -------------------------------------------------------------------

//@Render
bool Renderer::GetRenderLights() { return renderer.renderLights; }
void Renderer::SetRenderLights(bool render) { renderer.renderLights = render; }

bool Renderer::GetRenderShadows() { return renderer.renderShadows; }
void Renderer::SetRenderShadows(bool render) { renderer.renderShadows = render; }

//@Draw
bool Renderer::GetDrawGrid() { return renderer.drawGrid; }
void Renderer::SetDrawGrid(bool draw) { renderer.drawGrid = draw; }

bool Renderer::GetDrawAxis() { return renderer.drawAxis; }
void Renderer::SetDrawAxis(bool draw) { renderer.drawAxis = draw; }

bool Renderer::GetDrawCollisions() { return renderer.drawCollisions; }
void Renderer::SetDrawCollisions(bool draw) { renderer.drawCollisions = draw; }

bool Renderer::GetDrawScriptShapes() { return renderer.drawScriptShapes; }
void Renderer::SetDrawScriptShapes(bool draw) { renderer.drawScriptShapes = draw; }

bool Renderer::GetDrawRaycasting() { return renderer.drawRaycasting; }
void Renderer::SetDrawRaycasting(bool draw) { renderer.drawRaycasting = draw; }


//@Draw Utils ----------------------------------------------------------------
void Renderer::DebugDraw(bool override)
{
    // Draw Editor / Debug
    if (renderer.drawAxis || override)
        DrawAxis();

    if (renderer.drawGrid || override)
        DrawGrid(5000, 50);

    if (renderer.drawCollisions || override)
        engine->collisionSolver->DrawCollisions();

    if (renderer.drawScriptShapes || override)
        engine->monoManager->RenderShapesQueue();

    if (override)
    {
        for (const auto GO : engine->N_sceneManager->GetGameObjects())
        {
            Camera* gameCam = GO.get()->GetComponent<Camera>();

            if (gameCam != nullptr && gameCam->drawFrustum)
                DrawFrustum(gameCam->frustum);
        }
    }

    // Draw Rays
    if (renderer.drawRaycasting || override)
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

    Renderer2D::DrawLine(BT::WORLD, origin, xaxis, { 1.0f, 0.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, origin, yaxis, { 0.0f, 1.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, origin, zaxis, { 0.0f, 0.0f, 1.0f, 1.0f });
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

        Renderer2D::DrawLine(BT::WORLD, v1, v2, color);
        Renderer2D::DrawLine(BT::WORLD, v3, v4, color);
    }
}

void Renderer::DrawFrustum(const Frustum& frustum)
{
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[0], frustum.vertices[1], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[1], frustum.vertices[2], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[2], frustum.vertices[3], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[3], frustum.vertices[0], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[4], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[5], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[6], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[7], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[0], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[1], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[2], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(BT::WORLD, frustum.vertices[3], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
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
    Renderer2D::DrawLine(BT::WORLD, { ray.Origin.x,  ray.Origin.y, ray.Origin.z },
        { ray.Origin.x + ray.Direction.x * 1000,
         ray.Origin.y + ray.Direction.y * 1000,
         ray.Origin.z + ray.Direction.z * 1000 }, { 1.0f, 1.0f, 1.0f, 1.0f });
}