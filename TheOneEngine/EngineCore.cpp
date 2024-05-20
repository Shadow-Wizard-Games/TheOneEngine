#include "EngineCore.h"
#include "Log.h"
#include "Defs.h"
#include "N_SceneManager.h"
#include "Collider2D.h"
#include "Renderer2D.h"
#include "Renderer3D.h"
#include <GL\glew.h>
#include <glm\ext\matrix_transform.hpp>
#include <IL\il.h>
#include <memory>
//#include "../TheOneEditor/App.h"
//#include "../TheOneEditor/SceneManager.h"

AudioManager* audioManager = NULL;

EngineCore::EngineCore()
{
    audioManager = new AudioManager();
    monoManager = new MonoManager();
    collisionSolver = new CollisionSolver();
    inputManager = new InputManager();
    N_sceneManager = new N_SceneManager();
    easingManager = new EasingManager();
}

void EngineCore::Awake()
{
    LOG(LogType::LOG_OK, "Initializing DevIL");
    ilInit();
    audioManager->Awake();
    monoManager->InitMono();
    inputManager->Init();
}

void EngineCore::Start()
{
    Renderer2D::Init();

    InitPreLightingShader();
    InitPostLightingShader(); 
    //InitLitMeshTextureAnimatedShaders();
    //InitLitMeshColorShaders();
    
    CameraUniformBuffer = std::make_shared<UniformBuffer>(sizeof(glm::mat4), 0);
}

bool EngineCore::PreUpdate()
{
    if (!inputManager->PreUpdate()) { return false; }
    if (!collisionSolver->PreUpdate()) { return false; }
    return true;
}

void EngineCore::Update(double dt)
{
    audioManager->Update(dt);
    collisionSolver->Update(dt);
    //easingManager->Update(dt);

    this->dt = dt;
}

void EngineCore::SetRenderEnvironment()
{
    GLCALL(glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST));
    //GLCALL(glClearDepth(1.0f));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthFunc(GL_LEQUAL));
    GLCALL(glEnable(GL_CULL_FACE));
    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCALL(glEnable(GL_COLOR_MATERIAL));

    GLCALL(glClear(GL_DEPTH_BUFFER_BIT));
}

void EngineCore::DebugDraw(bool override)
{
    // Draw Editor / Debug
    if (drawAxis || override)
        DrawAxis();

    if (drawGrid || override)
        DrawGrid(5000, 50);

    if (drawCollisions || override)
        collisionSolver->DrawCollisions();

    if (drawScriptShapes || override)
        monoManager->RenderShapesQueue();

    if (override)
    {
        for (const auto GO : engine->N_sceneManager->GetGameObjects())
        {
            Camera* gameCam = GO.get()->GetComponent<Camera>();

            if (gameCam != nullptr && gameCam->drawFrustum)
                engine->DrawFrustum(gameCam->frustum);
        }
    }

    // Draw Rays
    if (drawRaycasting || override)
    {
        for (auto ray : rays)
            engine->DrawRay(ray);
    }
}

void EngineCore::CleanUp()
{
    Renderer2D::Shutdown();
    Renderer3D::Shutdown();

    audioManager->CleanUp();
    audioManager = nullptr;
    delete audioManager;
    
    monoManager->ShutDownMono();
    monoManager = nullptr;
    delete monoManager;

    inputManager->CleanUp();
    inputManager = nullptr;
    delete inputManager;

    collisionSolver = nullptr;
    delete collisionSolver;
}

void EngineCore::DrawAxis()
{
    glm::vec3 origin = { 0.0f, 0.0f, 0.0f };
    glm::vec3 xaxis = { 0.8f, 0.0f, 0.0f };
    glm::vec3 yaxis = { 0.0f, 0.8f, 0.0f };
    glm::vec3 zaxis = { 0.0f, 0.0f, 0.8f };

    Renderer2D::DrawLine(origin, xaxis, { 1.0f, 0.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(origin, yaxis, { 0.0f, 1.0f, 0.0f, 1.0f });
    Renderer2D::DrawLine(origin, zaxis, { 0.0f, 0.0f, 1.0f, 1.0f });
}

void EngineCore::DrawGrid(int grid_size, int grid_step)
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
        glm::vec3 v4 = {  grid_size, 0.0f, i };
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, alpha };

        Renderer2D::DrawLine(v1, v2, color);
        Renderer2D::DrawLine(v3, v4, color);
    }
}

void EngineCore::DrawFrustum(const Frustum& frustum)
{
    Renderer2D::DrawLine(frustum.vertices[0], frustum.vertices[1], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[1], frustum.vertices[2], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[2], frustum.vertices[3], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[3], frustum.vertices[0], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(frustum.vertices[4], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[5], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[6], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[7], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });

    Renderer2D::DrawLine(frustum.vertices[0], frustum.vertices[4], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[1], frustum.vertices[5], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[2], frustum.vertices[6], { 1.0f, 1.0f, 1.0f, 1.0f });
    Renderer2D::DrawLine(frustum.vertices[3], frustum.vertices[7], { 1.0f, 1.0f, 1.0f, 1.0f });
}

void EngineCore::DrawRay(const Ray& ray)
{
    // Define vertex data
    GLfloat rayVertices[] = {
        ray.Origin.x, ray.Origin.y, ray.Origin.z,
        ray.Origin.x + ray.Direction.x * 1000,
        ray.Origin.y + ray.Direction.y * 1000,
        ray.Origin.z + ray.Direction.z * 1000
    };
    Renderer2D::DrawLine({ ray.Origin.x,  ray.Origin.y, ray.Origin.z },
                          {ray.Origin.x + ray.Direction.x * 1000,
                           ray.Origin.y + ray.Direction.y * 1000,
                           ray.Origin.z + ray.Direction.z * 1000}, { 1.0f, 1.0f, 1.0f, 1.0f });
}

bool EngineCore::GetVSync()
{
    return vsync;
}

//heakbs - parameter not used?
bool EngineCore::SetVSync(bool vsync)
{
    if (this->vsync)
    {
        if (SDL_GL_SetSwapInterval(1) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn on V-Sync: %s", SDL_GetError());
            return false;
        }
    }
    else
    {
        if (SDL_GL_SetSwapInterval(0) == -1)
        {
            LOG(LogType::LOG_ERROR, "Unable to turn off V-Sync: %s", SDL_GetError());
            return false;
        }
    }

    return true;
}

std::vector<LogInfo> EngineCore::GetLogs()
{
    return logs;
}

void EngineCore::AddLog(LogType type, const char* entry)
{
    if (logs.size() > MAX_LOGS_CONSOLE)
        logs.erase(logs.begin());

    std::string toAdd = entry;
    LogInfo info = { type, toAdd };

    logs.push_back(info);
}

void EngineCore::CleanLogs()
{
    logs.clear();
}

void EngineCore::SetEditorCamera(Camera* cam)
{
    if(cam)
        editorCamReference = cam;
}

void EngineCore::SetUniformBufferCamera(const glm::mat4& camMatrix)
{
    CameraUniformBuffer->SetData(&camMatrix, sizeof(glm::mat4));
}

void EngineCore::InitPreLightingShader()
{
    ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTexture");
    Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
    textShader->Compile("Assets/Shaders/MeshTexture");

    textShader->addUniform("diffuse", UniformType::Sampler2D);
    Resources::Import<Shader>("MeshTexture", textShader);


    ResourceId skeletalTextShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTextureAnimated");
    Shader* skeletalTextShader = Resources::GetResourceById<Shader>(skeletalTextShaderId);
    skeletalTextShader->Compile("Assets/Shaders/MeshTextureAnimated");

    skeletalTextShader->addUniform("diffuse", UniformType::Sampler2D);
    Resources::Import<Shader>("MeshTextureAnimated", skeletalTextShader);
}

void EngineCore::InitPostLightingShader()
{
    GLERR;
    //Init default shaders with uniforms
    ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/PostLightingShader");
    Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
    textShader->Compile("Assets/Shaders/PostLightingShader");
    textShader->addUniform("gPosition", UniformType::Sampler2D);
    textShader->addUniform("gNormal", UniformType::Sampler2D);
    textShader->addUniform("gAlbedoSpec", UniformType::Sampler2D);
    textShader->addUniform("u_ViewPos", UniformType::fVec3);
    textShader->addUniform("u_TotalLightsNum", UniformType::Int);

    for (uint i = 0; i < 5; i++)
    {
        string iteration = to_string(i);
        textShader->addUniform("u_DirLight[" + iteration + "].Position", UniformType::fVec3);
        textShader->addUniform("u_DirLight[" + iteration + "].Color", UniformType::fVec3);
        textShader->addUniform("u_DirLight[" + iteration + "].Direction", UniformType::fVec3);
    }

    for (uint i = 0; i < 32; i++)
    {
        string iteration = to_string(i);
        textShader->addUniform("u_PointLights[" + iteration + "].Position", UniformType::fVec3);
        textShader->addUniform("u_PointLights[" + iteration + "].Color", UniformType::fVec3);
        textShader->addUniform("u_PointLights[" + iteration + "].Linear", UniformType::Float);
        textShader->addUniform("u_PointLights[" + iteration + "].Quadratic", UniformType::Float);
        textShader->addUniform("u_PointLights[" + iteration + "].Radius", UniformType::Float);
    }

    for (uint i = 0; i < 32; i++)
    {
        string iteration = to_string(i);
        textShader->addUniform("u_SpotLights[" + iteration + "].Position", UniformType::fVec3);
        textShader->addUniform("u_SpotLights[" + iteration + "].Color", UniformType::fVec3);
        textShader->addUniform("u_SpotLights[" + iteration + "].Direction", UniformType::fVec3);
        textShader->addUniform("u_SpotLights[" + iteration + "].Linear", UniformType::Float);
        textShader->addUniform("u_SpotLights[" + iteration + "].Quadratic", UniformType::Float);
        textShader->addUniform("u_SpotLights[" + iteration + "].Radius", UniformType::Float);
        textShader->addUniform("u_SpotLights[" + iteration + "].CutOff", UniformType::Float);
        textShader->addUniform("u_SpotLights[" + iteration + "].OuterCutOff", UniformType::Float);
    }
    Resources::Import<Shader>("PostLightingShader", textShader);

    Material lightinProcessMat(textShader);
    lightinProcessMat.setShader(textShader, textShader->getPath());
    lightingProcessPath = Resources::PathToLibrary<Material>() + "lightingProcess.toematerial";
    Resources::Import<Material>(lightingProcessPath, &lightinProcessMat);
    Resources::LoadFromLibrary<Material>(lightingProcessPath);
}

void EngineCore::InitLitMeshColorShaders()
{
    ResourceId colorShaderId = Resources::Load<Shader>("Assets/Shaders/LitMeshColor");
    Shader* colorShader = Resources::GetResourceById<Shader>(colorShaderId);
    colorShader->Compile("Assets/Shaders/LitMeshColor");
    colorShader->addUniform("u_PointLightsNum", UniformType::Int);
    for (uint i = 0; i < 32; i++)
    {
        string iteration = to_string(i);
        colorShader->addUniform("u_PointLights[" + iteration + "].position", UniformType::fVec3);
        colorShader->addUniform("u_PointLights[" + iteration + "].constant", UniformType::Float);
        colorShader->addUniform("u_PointLights[" + iteration + "].linear", UniformType::Float);
        colorShader->addUniform("u_PointLights[" + iteration + "].quadratic", UniformType::Float);
        colorShader->addUniform("u_PointLights[" + iteration + "].ambient", UniformType::fVec3);
        colorShader->addUniform("u_PointLights[" + iteration + "].diffuse", UniformType::fVec3);
        colorShader->addUniform("u_PointLights[" + iteration + "].specular", UniformType::fVec3);
    }
    colorShader->addUniform("u_SpotLightsNum", UniformType::Int);
    for (uint i = 0; i < 12; i++)
    {
        string iteration = to_string(i);
        colorShader->addUniform("u_SpotLights[" + iteration + "].position", UniformType::fVec3);
        colorShader->addUniform("u_SpotLights[" + iteration + "].direction", UniformType::fVec3);
        colorShader->addUniform("u_SpotLights[" + iteration + "].cutOff", UniformType::Float);
        colorShader->addUniform("u_SpotLights[" + iteration + "].outerCutOff", UniformType::Float);
        colorShader->addUniform("u_SpotLights[" + iteration + "].ambient", UniformType::fVec3);
        colorShader->addUniform("u_SpotLights[" + iteration + "].diffuse", UniformType::fVec3);
        colorShader->addUniform("u_SpotLights[" + iteration + "].specular", UniformType::fVec3);
        colorShader->addUniform("u_SpotLights[" + iteration + "].constant", UniformType::Float);
        colorShader->addUniform("u_SpotLights[" + iteration + "].linear", UniformType::Float);
        colorShader->addUniform("u_SpotLights[" + iteration + "].quadratic", UniformType::Float);
        
    }
    colorShader->addUniform("u_ViewPos", UniformType::fVec3);
    colorShader->addUniform("u_Material.diffuse", UniformType::fVec3);
    colorShader->addUniform("u_Material.specular", UniformType::fVec3);
    colorShader->addUniform("u_Material.shininess", UniformType::Float);
    Resources::Import<Shader>("LitMeshColor", colorShader);

    //Default Material
    Material defaultMat(colorShader);
    defaultMat.SetUniformData("u_Material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    std::string matPath = Resources::PathToLibrary<Material>() + "defaultMat.toematerial";
    Resources::Import<Material>(matPath, &defaultMat);
    Resources::LoadFromLibrary<Material>(matPath);
}