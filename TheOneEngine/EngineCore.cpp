#include "EngineCore.h"
#include "Log.h"
#include "Defs.h"
#include "N_SceneManager.h"
#include "Collider2D.h"
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
    
}

bool EngineCore::PreUpdate()
{
    inputManager->PreUpdate();
    collisionSolver->PreUpdate();
    return true;
}

void EngineCore::Update(double dt)
{
    
    collisionSolver->Update(dt);
    audioManager->Update(dt);

    this->dt = dt;
}

void EngineCore::Render(Camera* camera)
{
    LogGL("BEFORE RENDER");

    if (!camera) camera = editorCamReference;

    // Update Camera Matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);

    switch (camera->cameraType)
    {
    case CameraType::PERSPECTIVE:
        gluPerspective(camera->fov, camera->aspect, camera->zNear, camera->zFar);
        break;
    case CameraType::ORTHOGONAL:
        glOrtho(-camera->size, camera->size, -camera->size * 0.75, camera->size * 0.75, camera->zNear, camera->zFar);
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

    //DrawGrid(1000, 50);
    //DrawAxis();

    //if (collisionSolver->drawCollisions) collisionSolver->DrawCollisions();

    /*if (!monoManager->debugShapesQueue.empty())
    {
        monoManager->RenderShapesQueue();
    }*/

    glColor3f(1.0f, 1.0f, 1.0f);
    //DrawFrustum(camera->viewMatrix);

    LogGL("DURING RENDER");

    assert(glGetError() == GL_NONE);
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
    audioManager->CleanUp();
    delete audioManager;
    
    monoManager->ShutDownMono();
    delete monoManager;

    inputManager->CleanUp();
    delete inputManager;

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

    // Create and bind vertex array object (VAO)
    GLuint axisVAO;
    glGenVertexArrays(1, &axisVAO);
    glBindVertexArray(axisVAO);

    // Create and bind vertex buffer object (VBO) for vertices
    GLuint vertexVBO;
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Create and bind vertex buffer object (VBO) for colors
    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Draw lines
    glDrawArrays(GL_LINES, 0, 6);

    // Cleanup
    glDeleteBuffers(1, &vertexVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteVertexArrays(1, &axisVAO);
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

    // Create and bind vertex array object (VAO)
    GLuint gridVAO;
    glGenVertexArrays(1, &gridVAO);
    glBindVertexArray(gridVAO);

    // Create and bind vertex buffer object (VBO) for vertices
    GLuint gridVBO;
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(GLfloat), gridVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Create and bind vertex buffer object (VBO) for colors
    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, gridColors.size() * sizeof(GLfloat), gridColors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Draw lines
    glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);

    // Cleanup
    glDeleteBuffers(1, &gridVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteVertexArrays(1, &gridVAO);
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

    // Create and bind vertex array object (VAO)
    GLuint frustumVAO;
    glGenVertexArrays(1, &frustumVAO);
    glBindVertexArray(frustumVAO);

    // Create and bind vertex buffer object (VBO) for vertices
    GLuint frustumVBO;
    glGenBuffers(1, &frustumVBO);
    glBindBuffer(GL_ARRAY_BUFFER, frustumVBO);
    glBufferData(GL_ARRAY_BUFFER, frustumVertices.size() * sizeof(GLfloat), frustumVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Create and bind element buffer object (EBO) for indices
    GLuint frustumEBO;
    glGenBuffers(1, &frustumEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frustumIndices), frustumIndices, GL_STATIC_DRAW);

    // Draw lines
    glDrawElements(GL_LINES, sizeof(frustumIndices) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);

    // Cleanup
    glDeleteBuffers(1, &frustumVBO);
    glDeleteBuffers(1, &frustumEBO);
    glDeleteVertexArrays(1, &frustumVAO);
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

    // Create and bind vertex array object (VAO)
    GLuint rayVAO;
    glGenVertexArrays(1, &rayVAO);
    glBindVertexArray(rayVAO);

    // Create and bind vertex buffer object (VBO) for vertices
    GLuint rayVBO;
    glGenBuffers(1, &rayVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rayVertices), rayVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Draw line
    glDrawArrays(GL_LINES, 0, 2);

    // Cleanup
    glDeleteBuffers(1, &rayVBO);
    glDeleteVertexArrays(1, &rayVAO);
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
