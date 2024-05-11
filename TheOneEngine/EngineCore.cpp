#include "EngineCore.h"
#include "Log.h"
#include "Defs.h"
#include "N_SceneManager.h"
#include "Collider2D.h"
#include "Renderer2D.h"
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

    //Init default shaders with uniforms
    ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTexture");
    Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
    textShader->Compile("Assets/Shaders/MeshTexture");
    textShader->addUniform("u_Tex", UniformType::Sampler2D);
    Resources::Import<Shader>("MeshTexture", textShader);

    ResourceId colorShaderId = Resources::Load<Shader>("Assets/Shaders/MeshColor");
    Shader* colorShader = Resources::GetResourceById<Shader>(colorShaderId);
    colorShader->Compile("Assets/Shaders/MeshColor");
    colorShader->addUniform("u_Color", UniformType::fVec4);
    Resources::Import<Shader>("MeshColor", colorShader);

    ResourceId animTextShaderId = Resources::Load<Shader>("Assets/Shaders/MeshTextureAnimated");
    Shader* animTextShader = Resources::GetResourceById<Shader>(animTextShaderId);
    animTextShader->Compile("Assets/Shaders/MeshTextureAnimated");
    animTextShader->addUniform("u_Tex", UniformType::Sampler2D);
    Resources::Import<Shader>("MeshTextureAnimated", animTextShader);

    //Default Material
    Material defaultMat(colorShader);
    defaultMat.SetUniformData("u_Color", glm::vec4(1, 0, 1, 1));
    std::string matPath = Resources::PathToLibrary<Material>() + "defaultMat.toematerial";
    Resources::Import<Material>(matPath, &defaultMat);
    Resources::LoadFromLibrary<Material>(matPath);

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
    GLCALL(glClearDepth(1.0f));

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
    Renderer2D::StartBatch();
    // Draw Editor / Debug
    if (drawAxis || override)
        DrawAxis();

    if (drawGrid || override)
        DrawGrid(5000, 50);

    if (drawCollisions || override)
        collisionSolver->DrawCollisions();

    if (drawScriptShapes || override)
        monoManager->RenderShapesQueue();
    Renderer2D::Flush();
}

void EngineCore::CleanUp()
{
    Renderer2D::Shutdown();

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
    // Define vertex data
    std::vector<GLfloat> frustumVertices;
    for (int i = 0; i < 8; ++i) {
        frustumVertices.push_back(frustum.vertices[i].x);
        frustumVertices.push_back(frustum.vertices[i].y);
        frustumVertices.push_back(frustum.vertices[i].z);
    }

    // Define indices for the lines
    GLuint frustumIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0, // near plane
        4, 5, 5, 6, 6, 7, 7, 4, // far plane
        0, 4, 1, 5, 2, 6, 3, 7  // connecting lines
    };

    // Create and Bind vertex array object (VAO)
    GLuint frustumVAO;
    GLCALL(glGenVertexArrays(1, &frustumVAO));
    GLCALL(glBindVertexArray(frustumVAO));

    // Create and Bind vertex buffer object (VBO) for vertices
    GLuint frustumVBO;
    GLCALL(glGenBuffers(1, &frustumVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, frustumVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, frustumVertices.size() * sizeof(GLfloat), frustumVertices.data(), GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(0));

    // Create and Bind element buffer object (EBO) for indices
    GLuint frustumEBO;
    GLCALL(glGenBuffers(1, &frustumEBO));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumEBO));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustumIndices), frustumIndices, GL_STATIC_DRAW));

    // Draw lines
    GLCALL(glDrawElements(GL_LINES, sizeof(frustumIndices) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr));

    // Cleanup
    GLCALL(glDeleteBuffers(1, &frustumVBO));
    GLCALL(glDeleteBuffers(1, &frustumEBO));
    GLCALL(glDeleteVertexArrays(1, &frustumVAO));
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
