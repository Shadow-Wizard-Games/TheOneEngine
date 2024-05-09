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

    InitPreLightingShader();
    InitPostLightingShader();
    //InitLitMeshTextureAnimatedShaders();
    InitLitMeshColorShaders();
    
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

void EngineCore::SetRenderEnvironment(Camera* camera)
{
    if (!camera) camera = editorCamReference;

    // Update Camera Matrix
    GLCALL(glMatrixMode(GL_PROJECTION));
    GLCALL(glLoadIdentity());

    GLCALL(glMatrixMode(GL_MODELVIEW));
    GLCALL(glLoadIdentity());

    GLCALL(glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST));
    GLCALL(glClearDepth(1.0f));

    GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthFunc(GL_LEQUAL));
    GLCALL(glEnable(GL_CULL_FACE));
    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCALL(glEnable(GL_COLOR_MATERIAL));

    glClear(GL_DEPTH_BUFFER_BIT);

    switch (camera->cameraType)
    {
    case CameraType::PERSPECTIVE:
        gluPerspective(camera->fov, camera->aspect, camera->zNear, camera->zFar);
        break;

    case CameraType::ORTHOGRAPHIC:
    {
        float halfWidth = camera->size * 0.5f;
        float halfHeight = halfWidth / camera->aspect;
        GLCALL(glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, camera->zNear, camera->zFar));
    }
    break;

    default:
        LOG(LogType::LOG_ERROR, "EngineCore - CameraType invalid!");
        break;
    }

	Transform* cameraTransform = camera->GetContainerGO().get()->GetComponent<Transform>();

    if (cameraTransform == nullptr)
    {
        LOG(LogType::LOG_ERROR, "Camera Transform not found");
    }

    gluLookAt(cameraTransform->GetPosition().x, cameraTransform->GetPosition().y, cameraTransform->GetPosition().z,
        camera->lookAt.x, camera->lookAt.y, camera->lookAt.z,
		cameraTransform->GetUp().x, cameraTransform->GetUp().y, cameraTransform->GetUp().z);

    //GLCALL(glColor3f(1.0f, 1.0f, 1.0f));

    assert(glGetError() == GL_NONE);
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
}

void EngineCore::LogGL(string id)
{
    GLenum errorCode = glGetError();
    
    if (errorCode != GL_NO_ERROR)
    {
        const GLubyte* errorString = gluErrorString(errorCode);
        LOG(LogType::LOG_ERROR, "GL_%d  %s  %s", errorCode, errorString, id.c_str());
    }
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
    // Define vertex and color data
    GLfloat axisVertices[] = {
        0.0f, 0.0f, 0.0f, // origin
        0.8f, 0.0f, 0.0f, // x-axis end
        0.0f, 0.0f, 0.0f, // origin
        0.0f, 0.8f, 0.0f, // y-axis end
        0.0f, 0.0f, 0.0f, // origin
        0.0f, 0.0f, 0.8f  // z-axis end
    };
    GLubyte axisColors[] = {
        255, 0, 0, // red
        0, 255, 0, // green
        0, 0, 255  // blue
    };

    // Create and Bind vertex array object (VAO)
    GLuint axisVAO;
    GLCALL(glGenVertexArrays(1, &axisVAO));
    GLCALL(glBindVertexArray(axisVAO));

    // Create and Bind vertex buffer object (VBO) for vertices
    GLuint vertexVBO;
    GLCALL(glGenBuffers(1, &vertexVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vertexVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(0));

    // Create and Bind vertex buffer object (VBO) for colors
    GLuint colorVBO;
    GLCALL(glGenBuffers(1, &colorVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, colorVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(1));

    // Draw lines
    GLCALL(glDrawArrays(GL_LINES, 0, 6));

    // Cleanup
    GLCALL(glDeleteBuffers(1, &vertexVBO));
    GLCALL(glDeleteBuffers(1, &colorVBO));
    GLCALL(glDeleteVertexArrays(1, &axisVAO));
}

void EngineCore::DrawGrid(int grid_size, int grid_step)
{
    // Define vertex data
    std::vector<GLfloat> gridVertices;
    std::vector<GLfloat> gridColors;

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
        //alpha = std::max(0.0f, std::min(alpha, 1.0f));

        // Add vertices
        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(-grid_size);

        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(grid_size);

        gridVertices.push_back(-grid_size);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);

        gridVertices.push_back(grid_size);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);

        // Add colors with adjusted alpha
        for (int j = 0; j < 4; ++j)
        {
            gridColors.push_back(128); // R
            gridColors.push_back(0); // G
            gridColors.push_back(128); // B
            gridColors.push_back(static_cast<GLfloat>(alpha * 255)); // Alpha
        }
    }

    // Create and Bind vertex array object (VAO)
    GLuint gridVAO;
    GLCALL(glGenVertexArrays(1, &gridVAO));
    GLCALL(glBindVertexArray(gridVAO));

    // Create and Bind vertex buffer object (VBO) for vertices
    GLuint gridVBO;
    GLCALL(glGenBuffers(1, &gridVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, gridVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GLfloat), gridVertices.data(), GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(0));

    // Create and Bind vertex buffer object (VBO) for colors
    GLuint colorVBO;
    GLCALL(glGenBuffers(1, &colorVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, colorVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, gridColors.size() * sizeof(GLfloat), gridColors.data(), GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(1));


    // Draw lines
    GLCALL(glColor3f(0.33f, 0.33f, 0.33f));
    GLCALL(glDrawArrays(GL_LINES, 0, gridVertices.size() / 3));

    // Cleanup
    GLCALL(glDeleteBuffers(1, &gridVBO));
    GLCALL(glDeleteBuffers(1, &colorVBO));
    GLCALL(glDeleteVertexArrays(1, &gridVAO));
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

    // Create and Bind vertex array object (VAO)
    GLuint rayVAO;
    GLCALL(glGenVertexArrays(1, &rayVAO));
    GLCALL(glBindVertexArray(rayVAO));

    // Create and Bind vertex buffer object (VBO) for vertices
    GLuint rayVBO;
    GLCALL(glGenBuffers(1, &rayVBO));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, rayVBO));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(rayVertices), rayVertices, GL_STATIC_DRAW));
    GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));
    GLCALL(glEnableVertexAttribArray(0));

    // Draw line
    GLCALL(glDrawArrays(GL_LINES, 0, 2));

    // Cleanup
    GLCALL(glDeleteBuffers(1, &rayVBO));
    GLCALL(glDeleteVertexArrays(1, &rayVAO));
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

void EngineCore::InitPostLightingShader()
{
    //Init default shaders with uniforms
    ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/PostLightingShader");
    Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
    textShader->Compile("Assets/Shaders/PostLightingShader");
    textShader->addUniform("gPosition", UniformType::Sampler2D);
    textShader->addUniform("gNormal", UniformType::Sampler2D);
    textShader->addUniform("gAlbedoSpec", UniformType::Sampler2D);
    textShader->addUniform("u_ViewPos", UniformType::fVec3);
    textShader->addUniform("u_PointLightsNum", UniformType::Int);
    for (uint i = 0; i < 32; i++)
    {
        string iteration = to_string(i);
        textShader->addUniform("u_PointLights[" + iteration + "].Position", UniformType::fVec3);
        textShader->addUniform("u_PointLights[" + iteration + "].Color", UniformType::fVec3);
        textShader->addUniform("u_PointLights[" + iteration + "].Linear", UniformType::Float);
        textShader->addUniform("u_PointLights[" + iteration + "].Quadratic", UniformType::Float);
        textShader->addUniform("u_PointLights[" + iteration + "].Radius", UniformType::Float);
    }
    textShader->addUniform("u_SpotLightsNum", UniformType::Int);
    for (uint i = 0; i < 12; i++)
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

    lightingProcess.setShader(textShader, textShader->getPath());
    /*Uniform::SamplerData gPositionData;
    Uniform::SamplerData gNormalData;
    Uniform::SamplerData gAlbedoSpecData;*/
    /*lightingProcess.SetUniformData("gPosition", gPositionData);
    lightingProcess.SetUniformData("gNormal", gNormalData);
    lightingProcess.SetUniformData("gAlbedoSpec", gAlbedoSpecData);*/
    std::string lightPath = Resources::PathToLibrary<Material>() + "lightingProcess.toematerial";
    Resources::Import<Material>(lightPath, &lightingProcess);
    Resources::LoadFromLibrary<Material>(lightPath);
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
    defaultMat.SetUniformData("u_Material.diffuse", glm::vec3(1.0f, 0, 1.0f));
    std::string matPath = Resources::PathToLibrary<Material>() + "defaultMat.toematerial";
    Resources::Import<Material>(matPath, &defaultMat);
    Resources::LoadFromLibrary<Material>(matPath);
}

void EngineCore::InitLitMeshTextureAnimatedShaders()
{
    ResourceId animTextShaderId = Resources::Load<Shader>("Assets/Shaders/LitMeshTextureAnimated");
    Shader* animTextShader = Resources::GetResourceById<Shader>(animTextShaderId);
    animTextShader->Compile("Assets/Shaders/LitMeshTextureAnimated");
    animTextShader->addUniform("u_PointLightsNum", UniformType::Int);
    for (uint i = 0; i < 32; i++)
    {
        string iteration = to_string(i);
        animTextShader->addUniform("u_PointLights[" + iteration + "].position", UniformType::fVec3);
        animTextShader->addUniform("u_PointLights[" + iteration + "].flux", UniformType::Float);
        animTextShader->addUniform("u_PointLights[" + iteration + "].ambient", UniformType::fVec3);
        animTextShader->addUniform("u_PointLights[" + iteration + "].diffuse", UniformType::fVec3);
        animTextShader->addUniform("u_PointLights[" + iteration + "].specular", UniformType::fVec3);
    }
    animTextShader->addUniform("u_SpotLightsNum", UniformType::Int);
    for (uint i = 0; i < 12; i++)
    {
        string iteration = to_string(i);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].position", UniformType::fVec3);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].direction", UniformType::fVec3);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].cutOff", UniformType::Float);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].outerCutOff", UniformType::Float);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].ambient", UniformType::fVec3);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].diffuse", UniformType::fVec3);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].specular", UniformType::fVec3);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].constant", UniformType::Float);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].linear", UniformType::Float);
        animTextShader->addUniform("u_SpotLights[" + iteration + "].quadratic", UniformType::Float);
    }
    animTextShader->addUniform("u_ViewPos", UniformType::fVec3);
    animTextShader->addUniform("u_Material.diffuse", UniformType::Sampler2D);
    animTextShader->addUniform("u_Material.specular", UniformType::fVec3);
    animTextShader->addUniform("u_Material.shininess", UniformType::Float);
    Resources::Import<Shader>("LitMeshTextureAnimated", animTextShader);
}

void EngineCore::InitPreLightingShader()
{
    //Init default shaders with uniforms
    ResourceId textShaderId = Resources::Load<Shader>("Assets/Shaders/PreLightingShader");
    Shader* textShader = Resources::GetResourceById<Shader>(textShaderId);
    textShader->Compile("Assets/Shaders/PreLightingShader");

    textShader->addUniform("diffuse", UniformType::Sampler2D);
    Resources::Import<Shader>("PreLightingShader", textShader);
}
