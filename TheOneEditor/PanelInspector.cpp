#include "PanelInspector.h"
#include "App.h"
#include "Gui.h"
#include "PanelSettings.h"
#include "SceneManager.h"
#include "InspectorParticleSystems.h"

#include "TheOneEngine\Log.h"
#include "TheOneEngine\GameObject.h"
#include "TheOneEngine\Transform.h"
#include "TheOneEngine\Mesh.h"
#include "TheOneEngine\Camera.h"
#include "TheOneEngine\Script.h"
#include "TheOneEngine\Collider2D.h"
#include "TheOneEngine\Listener.h"
#include "TheOneEngine\AudioSource.h"
#include "TheOneEngine\Light.h"

#include "TheOneEngine\MonoManager.h"
#include "TheOneEngine\ParticleSystem.h"
#include "TheOneEngine\Canvas.h"
#include "TheOneEngine\ItemUI.h"
#include "TheOneEngine\ImageUI.h"
#include "TheOneEngine\ButtonImageUI.h"
#include "TheOneEngine\SliderUI.h"
#include "TheOneEngine\CheckerUI.h"
#include "TheOneEngine\TextUI.h"

#include "TheOneEngine\ResourcesImpl.h"
#include "TheOneEngine\FileDialog.h"

#include "TheOneAudio\AudioCore.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <fstream>

namespace fs = std::filesystem;

PanelInspector::PanelInspector(PanelType type, std::string name) : Panel(type, name)
{
    matrixDirty = false;
    chooseScriptNameWindow = false;
    chooseParticlesToImportWindow = false;
    view_pos = { 0, 0, 0 };
    view_rot_rad = { 0, 0, 0 };
    view_rot_deg = { 0, 0, 0 };
    view_sca = { 0, 0, 0 };
}

PanelInspector::~PanelInspector() {}

void PanelInspector::OnSelectGO(std::shared_ptr<GameObject> gameObj)
{   
    view_rot_rad = gameObj.get()->GetComponent<Transform>()->GetRotationEuler();
}

bool PanelInspector::Draw()
{
	ImGuiWindowFlags settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin("Inspector", &enabled, settingsFlags))
	{
        ImGuiIO& io = ImGui::GetIO();
        ImVec4 clear_color = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
        ImGui::SetNextWindowSize(ImVec2(250, 650), ImGuiCond_Once);

        selectedGO = engine->N_sceneManager->GetSelectedGO().get();

        if (selectedGO != nullptr)
        {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen;

            /*Name*/
            //ImGui::Checkbox("Active", &gameObjSelected->isActive);
            ImGui::SameLine(); ImGui::Text("GameObject:");
            ImGui::SameLine(); ImGui::TextColored({ 0.144f, 0.422f, 0.720f, 1.0f }, selectedGO->GetName().c_str());
            if (selectedGO->IsPrefab()/* && selectedGO->IsPrefabDirty()*/)
            {
                ImGui::SameLine(0.0f, -1.0f);
                if (ImGui::Button("Overrides", { 75.0f, 20.0f }))
                {
                    ImGui::OpenPopup("ChangesWarning");

                    OverridePrefabs(*selectedGO);
                }
            }

            bool isEnabled = selectedGO->IsEnabled();
            if (ImGui::Checkbox("Enable", &isEnabled)) {
                if (isEnabled != selectedGO->IsEnabled())
                {
                    if (isEnabled)
                        selectedGO->Enable();
                    else
                        selectedGO->Disable();
                }
            }

            ImGui::SameLine();
            ImGui::Checkbox("Keep GO", &selectedGO->isKeeped);

            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Tag + Layer*/
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //add change name imgui
            static char newNameBuffer[32]; // Buffer para el nuevo nombre
            strcpy(newNameBuffer, selectedGO->GetName().c_str());
            if (ImGui::InputText("New Name", newNameBuffer, sizeof(newNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                std::string newName(newNameBuffer);
                LOG(LogType::LOG_INFO, "GameObject %s has been renamed to %s", selectedGO->GetName().c_str(), newName.c_str());
                selectedGO->SetName(newName); // Establece el nuevo nombre del GameObject
                // Limpiar el buffer despu�s de cambiar el nombre
                newNameBuffer[0] = '\0';
            }

            ImGui::Checkbox("Has Transparency", &selectedGO->hasTransparency);

            
            /*Transform Component*/
            Transform* transform = selectedGO->GetComponent<Transform>();

            if (transform != nullptr && ImGui::CollapsingHeader("Transform", treeNodeFlags))
            {
                ImGui::SetItemTooltip("Displays and sets game object transformations");

                matrixDirty = false;

                view_pos = transform->GetPosition();
                view_rot_deg = ToDegrees(view_rot_rad);
                //vec3f initialRotation = view_rot_deg;
                view_sca = transform->GetScale();


                // Transform table ----------------------------------------------------------------------------------
                ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable;// | ImGuiTableFlags_SizingFixedFit;
                //ImGui::Indent(0.8f);
                if (ImGui::BeginTable("##transformTable", 4))
                {
                    ImGui::TableSetupColumn("##title");
                    ImGui::TableSetupColumn("##X");
                    ImGui::TableSetupColumn("##Y");
                    ImGui::TableSetupColumn("##Z");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushItemWidth(-FLT_MIN);

                    // Column 0: Labels
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Dummy({ 0, 1 });
                    ImGui::Text("Position");
                    ImGui::Dummy({ 0, 4 });
                    ImGui::Text("Rotation");
                    ImGui::Dummy({ 0, 4 });
                    ImGui::Text("Scale");

                    // Column 1: X Axis
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,         { 0.5f, 0.32f, 0.32f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  { 0.6f, 0.42f, 0.42f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   { 0.5f, 0.22f, 0.22f, 1.0f });
                    if (ImGui::DragFloat("X##PosX", &view_pos.x, 0.5F, 0, 0, "%.3f"))       matrixDirty = true;
                    if (ImGui::DragFloat("X##RotX", &view_rot_deg.x, 0.2f, 0, 0, "%.3f"))   matrixDirty = true;
                    if (ImGui::DragFloat("X##ScaleX", &view_sca.x, 0.1F, 0, 0, "%.3f"))     matrixDirty = true;
                    ImGui::PopStyleColor(3);

                    // Column 2: Y Axis
                    ImGui::TableSetColumnIndex(2);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,         { 0.32f, 0.5f, 0.32f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  { 0.42f, 0.6f, 0.42f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   { 0.22f, 0.5f, 0.22f, 1.0f });
                    if (ImGui::DragFloat("Y##PosY", &view_pos.y, 0.5F, 0, 0, "%.3f"))       matrixDirty = true;
                    if (ImGui::DragFloat("Y##RotY", &view_rot_deg.y, 0.2f, 0, 0, "%.3f"))   matrixDirty = true;
                    if (ImGui::DragFloat("Y##ScaleY", &view_sca.y, 0.1F, 0, 0, "%.3f"))     matrixDirty = true;
                    ImGui::PopStyleColor(3);

                    // Column 3: Z Axis
                    ImGui::TableSetColumnIndex(3);
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,         { 0.32f, 0.32f, 0.5f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,  { 0.42f, 0.42f, 0.6f, 1.0f });
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,   { 0.22f, 0.22f, 0.5f, 1.0f });
                    if (ImGui::DragFloat("Z##PosZ", &view_pos.z, 0.5F, 0, 0, "%.3f"))       matrixDirty = true;
                    if (ImGui::DragFloat("Z##RotZ", &view_rot_deg.z, 0.2f, 0, 0, "%.3f"))   matrixDirty = true;
                    if (ImGui::DragFloat("Z##ScaleZ", &view_sca.z, 0.1F, 0, 0, "%.3f"))     matrixDirty = true;
                    ImGui::PopStyleColor(3);

                    ImGui::EndTable();
                }

                if (matrixDirty)
                {
                    // translate -> rotate -> scale

                    transform->SetPosition(view_pos, HandleSpace::LOCAL);
                    //vec3f deltaRot = view_rot_deg - initialRotation;
                    view_rot_rad = ToRadians(view_rot_deg);
                    transform->SetRotation(view_rot_rad);

                    transform->SetScale(view_sca);
                }


                // Transform DEBUG 
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                ImGui::Indent(0.8f);
                if (ImGui::TreeNodeEx("Debug"))
                {
                    // Display transformMatrix
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::Text("Transform Matrix:");
                    for (int i = 0; i < 4; ++i) {
                        ImGui::Text("%f %f %f %f", transform->GetTransform()[i][0], transform->GetTransform()[i][1], transform->GetTransform()[i][2], transform->GetTransform()[i][3]);
                    }

                    // Display position
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::Text("Position: %.2f, %.2f, %.2f", transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);

                    // Display rotation (as euler angles)
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    glm::vec3 euler = glm::degrees(glm::eulerAngles(transform->GetRotation()));
                    ImGui::Text("Rotation: %.2f, %.2f, %.2f", view_rot_deg.x, view_rot_deg.y, view_rot_deg.z);

                    // Display scale
                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                    ImGui::Text("Scale: %.2f, %.2f, %.2f", transform->GetScale().x, transform->GetScale().y, transform->GetScale().z);

                    ImGui::TreePop();
                }


                //Check if camera needs to be updated
                Camera* camera = selectedGO->GetComponent<Camera>();
                if (camera && (matrixDirty))
                    camera->UpdateCamera();
                

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }


            /*Mesh Component*/
            Mesh* mesh = selectedGO->GetComponent<Mesh>();

            if (mesh != nullptr && ImGui::CollapsingHeader("Mesh", treeNodeFlags))
            {
                ImGui::SetItemTooltip("Displays and sets mesh data");
                //ImGui::Checkbox("Active", &mesh->isActive);
                //ImGui::SameLine();  
                ImGui::Text("Name: ");
                ImGui::SameLine();
                if (mesh->meshType == MeshType::DEFAULT) {
                    Model* model = Resources::GetResourceById<Model>(mesh->meshID);
                    ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, model->GetMeshName().c_str());
                }
                ImGui::Separator();

                ImGui::Checkbox("Mesh", &mesh->active);
                ImGui::Checkbox("Vertex normals", &mesh->drawNormalsVerts);
                ImGui::Checkbox("Face normals", &mesh->drawNormalsFaces);
                ImGui::Checkbox("Wireframe", &mesh->drawWireframe);
                ImGui::Checkbox("AABB", &mesh->drawAABB);
                ImGui::Checkbox("OBB", &mesh->drawOBB);
                //ImGui::Checkbox("Active checkboard", &mesh->drawChecker);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            

            /*Material Component*/
            //hekbas todo


            /*Texture Component*/
            //Texture* texture = selectedGO->GetComponent<Texture>();

            //if (texture != nullptr && ImGui::CollapsingHeader("Texture", treeNodeFlags))
            //{
            //    ImGui::SetItemTooltip("Displays and sets texture data");
            //    ImGui::Checkbox("Active Texture", &texture->active);
            //    ImGui::Text("Name: ");
            //    ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, (texture->GetName()).c_str());
            //    ImGui::Separator();
            //    ImGui::Text("Size: ");
            //    ImGui::SameLine();  ImGui::Text(std::to_string(texture->width).c_str());
            //    ImGui::Text("Height: ");
            //    ImGui::SameLine();  ImGui::Text(std::to_string(texture->height).c_str());

            //    //ImGui::Text("Tex coords: ");
            //    //ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.getNumTexCoords()).c_str());

            //    //if (ImGui::Checkbox("Use Texture", /*&mesh->usingTexture*/true))
            //    //{
            //    //    //(mesh->usingTexture) ? mesh->texture = gameObjSelected->GetComponent<Texture2D>() : mesh->texture = nullptr;
            //    //}

            //    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "%dpx x %dpx", s->getTexture()->width, s->getTexture()->height);

            //    // JULS: To show the image of the texture, but need to look at it more.
            //    //ImTextureID my_tex_id;
            //    //glGenTextures(GL_TEXTURE_2D, 1, my_tex_id);
            //    //glTextureParameteri(my_tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //    //glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //    //glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            //    //glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            //    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, [width of your texture], [height of your texture], false, GL_RGBA, GL_FLOAT, [pointer to first element in array of texture pixel values]);
            //    //ImGui::Image()

            //    ImGui::Dummy(ImVec2(0.0f, 10.0f));
            //}


            /*Camera Component*/
            Camera* camera = selectedGO->GetComponent<Camera>();

            if (camera != nullptr && ImGui::CollapsingHeader("Camera", treeNodeFlags))
            {
                bool isDirty = false;
                
                float fov = static_cast<float>(camera->fov);
                float aspect = static_cast<float>(camera->aspect);
                float size = static_cast<float>(camera->size);
                float zNear = static_cast<float>(camera->zNear);
                float zFar = static_cast<float>(camera->zFar);
                if (ImGui::BeginCombo("Camera Type", camera->cameraType == CameraType::PERSPECTIVE ? "Perspective" : "Orthographic"))
                {
                    if (ImGui::Selectable("Perspective", camera->cameraType == CameraType::PERSPECTIVE))
                    {
                        camera->cameraType = CameraType::PERSPECTIVE;
                        LOG(LogType::LOG_INFO, "Camera projection changed to PERSPECTIVE");
                        isDirty = true;
                    }

                    if (ImGui::Selectable("Orthogonal", camera->cameraType == CameraType::ORTHOGRAPHIC))
                    {
                        camera->cameraType = CameraType::ORTHOGRAPHIC;
                        LOG(LogType::LOG_INFO, "Camera projection changed to ORTHOGRAPHIC");
                        isDirty = true;
                    }

                    ImGui::EndCombo();
                }

                if (camera->cameraType == CameraType::PERSPECTIVE)
                {
                    if (ImGui::SliderFloat("FOV", &fov, 20.0, 120.0))
                    {
                        camera->fov = fov;
                        isDirty = true;
                    }

                    if (ImGui::SliderFloat("Aspect", &aspect, 0.1, 10.0))
                    {
                        camera->aspect = aspect;
                        isDirty = true;
                    }
                }
                
                if (camera->cameraType == CameraType::ORTHOGRAPHIC)
                {
                    if (ImGui::SliderFloat("SIZE", &size, 0.1, 500.0))
                    {
                        camera->size = size;
                        isDirty = true;
                    }
                }

                ImGui::Text("Clipping Planes");
                if (ImGui::SliderFloat("Near", &zNear, 0.01, 10.0))
                {
                    camera->zNear = zNear;
                    isDirty = true;
                }
                
                if (ImGui::SliderFloat("Far ", &zFar, 1.0, 10000.0))
                {
                    camera->zFar = zFar;
                    isDirty = true;
                }
                
                ImGui::Checkbox("Draw Frustrum", &camera->drawFrustum);

                if (ImGui::Checkbox("Primary Camera", &camera->primaryCam))
                {
                    engine->N_sceneManager->currentScene->ChangePrimaryCamera(selectedGO);
                }

                if (isDirty) camera->UpdateCamera();

                if (isDirty && selectedGO->IsPrefab())
                {
                    selectedGO->SetPrefabDirty(true);
                }

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            
            /*Light Component*/
            Light* light = selectedGO->GetComponent<Light>();

            if (light != nullptr && ImGui::CollapsingHeader("Light", treeNodeFlags))
            {
                // Type
                const char* label = nullptr;

                switch (light->lightType)
                {
                    case LightType::Point: label = "Point"; break;
                    case LightType::Spot: label = "Spot"; break;
                    case LightType::Directional: label = "Directional"; break;
                    default:break;
                }

                if (ImGui::BeginCombo("Type", label))
                {
                    if (ImGui::Selectable("Point"))
                        light->lightType = LightType::Point;

                    if (ImGui::Selectable("Spot"))
                        light->lightType = LightType::Spot;

                    if (ImGui::Selectable("Directional"))
                        light->lightType = LightType::Directional;

                    ImGui::EndCombo();
                }

                // Color
                ImVec4 color = ImVec4(light->color.r, light->color.g, light->color.b, 1.0f);

                static const char* color_id = "##lightColor3b";
                if (ImGui::ColorButton(color_id, color, app->gui->panelSettings->GetColorFlags(), ImVec2(ImGui::GetWindowWidth() * 0.65, 20)))
                {
                    app->gui->openColorPicker = true;
                    EditColor editColor = { color_id, color, color };
                    app->gui->SetEditColor(editColor);
                }
                else if (app->gui->GetEditColor().id == color_id)
                {
                    ImVec4 color = app->gui->GetEditColor().color;
                    light->color.r = color.x;
                    light->color.g = color.y;
                    light->color.b = color.z;
                }
                ImGui::SameLine();
                ImGui::SetCursorPos({ ImGui::GetCursorPosX() - 4, ImGui::GetCursorPosY() + 3 });
                ImGui::Text("Color");

                // Intensity
                ImGui::DragFloat("Intensity", &light->intensity, 0.1F, 0, 0, "%.3f");

                // Settings
                if (light->lightType == LightType::Point || light->lightType == LightType::Spot)
                {
                    ImGui::Text("Range");
                    ImGui::DragFloat("Radius", &light->radius, 0.5F, 0, 0, "%.3f");
                    ImGui::DragFloat("Linear", &light->linear, 0.02F, 0, 0, "%.3f");
                    ImGui::DragFloat("Quadratic", &light->quadratic, 0.02F, 0, 0, "%.3f");
                }

                if (light->lightType == LightType::Spot)
                {
                    ImGui::Text("Cut Off");
                    ImGui::DragFloat("Inner", &light->innerCutOff, 0.02F, 0, 0, "%.3f");
                    ImGui::DragFloat("Outer", &light->outerCutOff, 0.02F, 0, 0, "%.3f");
                }

                // Delete
                if (ImGui::Button("Delete"))
                    selectedGO->RemoveComponent(ComponentType::Light);

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }


            /*Script Component*/
            int id = 0;
            for (auto& script : selectedGO->GetComponents<Script>())
            {
                ImGuiTreeNodeFlags scriptTreeNodeFlags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_Leaf;
                if (script != nullptr && ImGui::CollapsingHeader("Script", scriptTreeNodeFlags))
                {
                    ImGui::TextColored({ 0.0f, 0.7f, 0.3f, 1.0f }, script->scriptName.c_str());
                    ImGui::SameLine();
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::PushID(id);
                    if (ImGui::Button("Remove"))
                        selectedGO->RemoveComponent(script);
                    ImGui::Dummy(ImVec2(0.0f, 10.0f));
                    ImGui::PopID();
                    id++;
                }
            }


            /*Collider2D Component*/
            Collider2D* collider2D = selectedGO->GetComponent<Collider2D>();

            if (collider2D != nullptr && ImGui::CollapsingHeader("Collider 2D", treeNodeFlags))
            {
                bool isDirty = false;

                // Collision type
                int collisionType = (int)collider2D->collisionType;
                ImGui::Text("Collision Type");
                ImGui::SameLine();
                const char* collisionTypes[] = { "Player", "Enemy", "Wall" };
                if (ImGui::Combo("##CollisionType", &collisionType, collisionTypes, 3))
                {
                    collider2D->collisionType = (CollisionType)collisionType;
                }


                // Collider type
                int colliderType = (int)collider2D->colliderType;
                ImGui::Text("Collider Type");
                ImGui::SameLine();
                if (ImGui::Combo("##ColliderType", &colliderType, colliders, 2))
                {
                    collider2D->colliderType = (ColliderType)colliderType;
                }

                // Offset of collider
                float offsetX = (float)collider2D->offset.x;
                float offsetY = (float)collider2D->offset.y;
                ImGui::Text("Offset");
                ImGui::Text("X");
                ImGui::SameLine();
                if (ImGui::InputFloat("##OffsetX", &offsetX))
                {
                    collider2D->offset.x = offsetX;
                    isDirty = true;
                }
                ImGui::Text("Y");
                ImGui::SameLine();
                if (ImGui::InputFloat("##OffsetY", &offsetY))
                {
                    collider2D->offset.y = offsetY;
                    isDirty = true;
                }

                if (collider2D->colliderType == ColliderType::Rect)
                {
                    if (ImGui::Checkbox("CornerPivot", &(collider2D->cornerPivot))) isDirty = true;

                    if (collider2D->cornerPivot)
                    {
                        int t = (int)collider2D->objectOrientation;
                        if (ImGui::Button("Change Collider Orientation"))
                        {
                            t++;
                            if (t >= 4)
                            {
                                t = 0;
                            }
                            collider2D->objectOrientation = (ObjectOrientation)t;
                        }
                    }

                    // Rectangle collider
                    float w = (float)collider2D->w;
                    float h = (float)collider2D->h;
                    ImGui::Text("Width");
                    ImGui::SameLine();
                    if (ImGui::InputFloat("##Width", &w))
                    {
                        collider2D->w = w;
                        isDirty = true;
                    }

                    ImGui::Text("Height");
                    ImGui::SameLine();
                    if (ImGui::InputFloat("##Height", &h))
                    {
                        collider2D->h = h;
                        isDirty = true;
                    }
                }
                else if (collider2D->colliderType == ColliderType::Circle)
                {
                    // Circle collider
                    float radius = collider2D->radius;
                    ImGui::Text("Radius");
                    ImGui::SameLine();
                    if (ImGui::InputFloat("##Radius", &radius))
                    {
                        collider2D->radius = radius;
                        isDirty = true;
                    }
                }

                if (ImGui::Button("Remove Collider"))
                {
                    selectedGO->RemoveComponent(ComponentType::Collider2D);
                    isDirty = true;
                }

                if (isDirty && selectedGO->IsPrefab())
                {
                    selectedGO->SetPrefabDirty(true);
                }

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }


            /*Particle System Component*/
            ParticleSystem* particleSystem = selectedGO->GetComponent<ParticleSystem>();

            if (particleSystem != nullptr && ImGui::CollapsingHeader("Particle System", treeNodeFlags))
            {
                // to see the particles without playing
                if (app->state == GameState::NONE) {
                    auto componentTransform = selectedGO->GetComponent<Transform>();
                    if (componentTransform != nullptr) {
                        componentTransform->CalculateWorldTransform();
                    }
                    particleSystem->Update(engine->dt);
                }
                bool isDirty = false;

                /*if (ImGui::Button("Load")) {
                    particleSystem->Load("");
                }
                ImGui::SameLine();
                if (ImGui::Button("Save")) {
                    particleSystem->Save();
                }*/

                if (!particleSystem->IsON()) {
                    if (ImGui::Button("Play")) {
                        particleSystem->Play();
                    }
                }
                else {
                    if (ImGui::Button("Pause")) {
                        particleSystem->Pause();
                    }
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Replay")) {
                    particleSystem->Replay();
                }
                ImGui::SameLine();
                if (ImGui::Button("Stop")) {
                    particleSystem->Stop();
                }

                ImGui::Checkbox("Start ON", &particleSystem->startON);

                if (ImGui::Button("Export")) {
                    particleSystem->ExportParticles();
                }
                ImGui::SameLine();
                if (ImGui::Button("Import")) {
                    chooseParticlesToImportWindow = true;
                    isDirty = true;
                }

                // change name
                particleSystem->GetNameToEdit()->resize(20);
                ImGui::InputText("Name", (char*)particleSystem->GetNameToEdit()->c_str(), 20);

                int emmiterID = 0;
                for (auto emmiter = particleSystem->emmiters.begin(); emmiter != particleSystem->emmiters.end(); ++emmiter)
                {
                    ImGui::PushID(emmiterID);
                    ImGui::Text("Emmiter %d", emmiterID);
                    // delete emmiter
                    UIEmmiterWriteNode((*emmiter).get());
                    emmiterID++;
                    ImGui::PopID();
                }
                
                // add emmiter
                if (ImGui::Button("Add Emmiter")) {
                    particleSystem->AddEmmiter();
                    isDirty = true;
                }

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::Button("Remove Particle System"))
                {
                    selectedGO->RemoveComponent(ComponentType::ParticleSystem);
                    isDirty = true;
                }

                if (isDirty && selectedGO->IsPrefab())
                {
                    selectedGO->SetPrefabDirty(true);
                }
            }


            // Canvas Component
            Canvas* tempCanvas = selectedGO->GetComponent<Canvas>();

            if (tempCanvas != nullptr && ImGui::CollapsingHeader("Canvas", treeNodeFlags))
            {
                bool componentState = tempCanvas->IsEnabled();
                if(ImGui::Checkbox("Enable/Disable Component", &(componentState)))
                {
                    if (componentState && !tempCanvas->IsEnabled())
                        tempCanvas->Enable();
                    else if (!componentState && tempCanvas->IsEnabled())
                        tempCanvas->Disable();
                }

                ImGui::Checkbox("Toggle debug draw", &(tempCanvas->debugDraw));

                //ui elements show
                int counter = 0;
                for (auto& item : tempCanvas->GetUiElements())
                {
                    std::string tstring = "  " + item->GetName() + " [id: " + std::to_string(item->GetID()) + "]";
                    if (item != nullptr && ImGui::CollapsingHeader(tstring.c_str(), treeNodeFlags))
                    {
                        unsigned int id = item->GetID();

                        //add change name imgui
                        static char changeUIName[32]; // Buffer para el nuevo nombre
                        strcpy(changeUIName, item->GetName().c_str());
                        if (ImGui::InputText(("Set Name##" + std::to_string(item->GetID())).c_str(), changeUIName, sizeof(changeUIName), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            std::string newName(changeUIName);
                            LOG(LogType::LOG_INFO, "ItemUI %s has been renamed to %s", item->GetName().c_str(), newName.c_str());
                            item->SetName(newName);
                            changeUIName[0] = '\0';
                        }
                        //move up/down in hierarchy
                        if (ImGui::ArrowButton(("Move Up in uiElements vector##" + std::to_string(item->GetID())).c_str(), 2) && counter > 0)
                        {
                            std::swap(tempCanvas->GetUiElementsPtr()[counter], tempCanvas->GetUiElementsPtr()[counter - 1]);
                            break;
                        }
                        ImGui::SameLine();
                        ImGui::Text("  Move Up");
                        if (ImGui::ArrowButton(("Move Down in uiElements vector##" + std::to_string(item->GetID())).c_str(), 3) && counter < tempCanvas->GetUiElements().size() - 1)
                        {
                            std::swap(tempCanvas->GetUiElementsPtr()[counter], tempCanvas->GetUiElementsPtr()[counter + 1]);
                            break;
                        }
                        ImGui::SameLine();
                        ImGui::Text("  Move Down");

                        bool print = item->IsPrintable();
                        ImGui::Checkbox(("Print##" + std::to_string(id)).c_str(), &print);
                        item->SetPrint(print);

                        std::string idstring = "Current ItemID is: " + std::to_string(item->GetID());
                        ImGui::Text(idstring.c_str());
                        ImGui::Text("Rect section info:");
                        float tempX, tempY, tempW, tempH;
                        tempX = item->GetRect().x;
                        tempY = item->GetRect().y;
                        tempW = item->GetRect().w;
                        tempH = item->GetRect().h;
                        ImGui::Text("   X:");
                        ImGui::SameLine();
                        ImGui::DragFloat(("##RectSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX, 0.005f, -2.0f, 2.0f);
                        ImGui::Text("   Y:");
                        ImGui::SameLine();
                        ImGui::DragFloat(("##RectSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY, 0.005f, -2.0f, 2.0f);
                        ImGui::Text("   W:");
                        ImGui::SameLine();
                        ImGui::DragFloat(("##RectSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW, 0.005f, 0.0f, 10.0f);
                        ImGui::Text("   H:");
                        ImGui::SameLine();
                        ImGui::DragFloat(("##RectSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH, 0.005f, 0.0f, 10.0f);
                        ImGui::Dummy(ImVec2(0.0f, 5.0f));

                        item->SetRect(tempX, tempY, tempW, tempH);

                        if (item->GetType() == UiType::IMAGE)
                        {
                            ImageUI* tempImageUI = tempCanvas->GetItemUI<ImageUI>(id);
                            ImGui::Text("UiType: IMAGE");
                            ImGui::Text("Image Path: %s", tempImageUI->GetPath().c_str());
                            ImGui::Text("Image section info:");

                            float tempX2, tempY2, tempW2, tempH2;
                            tempX2 = tempImageUI->GetSect().x;
                            tempY2 = tempImageUI->GetSect().y;
                            tempW2 = tempImageUI->GetSect().w;
                            tempH2 = tempImageUI->GetSect().h;
                            ImGui::Text("   X:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##ImageSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                            ImGui::Text("   Y:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##ImageSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                            ImGui::Text("   W:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##ImageSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                            ImGui::Text("   H:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##ImageSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                            tempImageUI->SetSectSize(tempX2, tempY2, tempW2, tempH2);
                        }
                        else if (item->GetType() == UiType::BUTTONIMAGE)
                        {
                            ButtonImageUI* tempButtonImageUI = tempCanvas->GetItemUI<ButtonImageUI>(id);
                            ImGui::Text("UiType: BUTTONIMAGE");
                            ImGui::Text("Image Path: %s", tempButtonImageUI->GetPath().c_str());

                            bool countAsRealButton = tempButtonImageUI->IsRealButton();
                            ImGui::Checkbox(("Count As Real Button##" + std::to_string(item->GetID())).c_str(), &countAsRealButton);
                            tempButtonImageUI->SetIsRealButton(countAsRealButton);

                            if (ImGui::CollapsingHeader("Image section IDLE info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as idle"))
                                {
                                    tempButtonImageUI->SetState(UiState::IDLE);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempButtonImageUI->GetSectIdle().x;
                                tempY2 = tempButtonImageUI->GetSectIdle().y;
                                tempW2 = tempButtonImageUI->GetSectIdle().w;
                                tempH2 = tempButtonImageUI->GetSectIdle().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageIdleSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageIdleSectionY of UIid" + std::to_string(item->GetID())).c_str(), & tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageIdleSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageIdleSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempButtonImageUI->GetState() == UiState::IDLE)
                                {
                                    tempButtonImageUI->SetSectSizeIdle(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                            if (ImGui::CollapsingHeader("Image section HOVERED info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as hovered"))
                                {
                                    tempButtonImageUI->SetState(UiState::HOVERED);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempButtonImageUI->GetSectHovered().x;
                                tempY2 = tempButtonImageUI->GetSectHovered().y;
                                tempW2 = tempButtonImageUI->GetSectHovered().w;
                                tempH2 = tempButtonImageUI->GetSectHovered().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageHoveredSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageHoveredSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageHoveredSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageHoveredSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempButtonImageUI->GetState() == UiState::HOVERED)
                                {
                                    tempButtonImageUI->SetSectSizeHovered(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                            if (ImGui::CollapsingHeader("Image section SELECTED info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as selected"))
                                {
                                    tempButtonImageUI->SetState(UiState::SELECTED);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempButtonImageUI->GetSectSelected().x;
                                tempY2 = tempButtonImageUI->GetSectSelected().y;
                                tempW2 = tempButtonImageUI->GetSectSelected().w;
                                tempH2 = tempButtonImageUI->GetSectSelected().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageSelectedSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageSelectedSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageSelectedSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##ImageSelectedSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempButtonImageUI->GetState() == UiState::SELECTED)
                                {
                                    tempButtonImageUI->SetSectSizeSelected(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                        }
                        else if (item->GetType() == UiType::SLIDER)
                        {
                            SliderUI* tempSliderUI = tempCanvas->GetItemUI<SliderUI>(id);
                            ImGui::Text("UiType: SLIDER");
                            if (ImGui::Button(("Change Slider Design##" + std::to_string(item->GetID())).c_str()))
                            {
                                int sliderDesign = (int)tempSliderUI->GetSliderDesign() + 1;
                                if (sliderDesign >= 2)
                                {
                                    sliderDesign = 0;
                                }
                                tempSliderUI->SetSliderDesign((SliderDesign)sliderDesign);
                            }
                            ImGui::SameLine();
                            switch (tempSliderUI->GetSliderDesign())
                            {
                            case SliderDesign::SAMEFOREACH:
                            ImGui::Text("Current SliderDesign: Same for each");
                                break;
                            case SliderDesign::DIFFERENTFIRSTANDLAST:
                            ImGui::Text("Current SliderDesign: Different First and Last");
                                break;
                            case SliderDesign::DEFAULT:
                            ImGui::Text("Current SliderDesign: Default");
                                break;
                            default:
                                break;
                            }
                            ImGui::Text("Image Path: %s", tempSliderUI->GetPath().c_str());

                            float tempOffset = tempSliderUI->GetOffset();
                            ImGui::DragFloat(("Slider Offset##" + std::to_string(item->GetID())).c_str(), &tempOffset, 0.002f, 0.0f);
                            tempSliderUI->SetOffset(tempOffset);

                            int tempCurrentValue = tempSliderUI->GetValue();
                            ImGui::DragInt(("Slider Current Value##" + std::to_string(item->GetID())).c_str(), &tempCurrentValue, 1, 0, tempSliderUI->GetMaxValue());
                            tempSliderUI->SetValue(tempCurrentValue);

                            int tempMaxValue = tempSliderUI->GetMaxValue();
                            ImGui::DragInt(("Slider Max Value##" + std::to_string(item->GetID())).c_str(), &tempMaxValue, 1, tempSliderUI->GetValue());
                            tempSliderUI->SetMaxValue(tempMaxValue);

                            if (tempSliderUI->GetSliderDesign() == SliderDesign::SAMEFOREACH)
                            {
                                if (ImGui::ArrowButton(("Change Section to edit Left 1##" + std::to_string(item->GetID())).c_str(), 0))
                                {
                                    sliderActivePart = !sliderActivePart;
                                }
                                ImGui::SameLine();
                                ImGui::Text(sliderActivePart ? "  Activated Part  " : "  Disabled Part  ");
                                ImGui::SameLine();
                                if (ImGui::ArrowButton(("Change Section to edit Right 1##" + std::to_string(item->GetID())).c_str(), 1))
                                {
                                    sliderActivePart = !sliderActivePart;
                                }

                                if (ImGui::CollapsingHeader("Image section IDLE info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as idle"))
                                    {
                                        tempSliderUI->SetState(UiState::IDLE);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectIdle(0, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectIdle(0, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectIdle(0, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectIdle(0, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderIdleSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderIdleSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderIdleSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderIdleSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::IDLE)
                                    {
                                        tempSliderUI->SetSectSizeIdle(tempX2, tempY2, tempW2, tempH2, 0, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                                if (ImGui::CollapsingHeader("Image section HOVERED info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as hovered"))
                                    {
                                        tempSliderUI->SetState(UiState::HOVERED);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectHovered(0, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectHovered(0, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectHovered(0, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectHovered(0, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderHoveredSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderHoveredSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderHoveredSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderHoveredSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::HOVERED)
                                    {
                                        tempSliderUI->SetSectSizeHovered(tempX2, tempY2, tempW2, tempH2, 0, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                                if (ImGui::CollapsingHeader("Image section SELECTED info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as selected"))
                                    {
                                        tempSliderUI->SetState(UiState::SELECTED);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectSelected(0, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectSelected(0, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectSelected(0, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectSelected(0, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderSelectedSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderSelectedSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderSelectedSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderSelectedSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::SELECTED)
                                    {
                                        tempSliderUI->SetSectSizeSelected(tempX2, tempY2, tempW2, tempH2, 0, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                            }
                            else if (tempSliderUI->GetSliderDesign() == SliderDesign::DIFFERENTFIRSTANDLAST)
                            {
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                float tempAdditionalOffsetFirst = tempSliderUI->GetAdditionalOffsetFirst();
                                ImGui::DragFloat("Slider First Additional Offset", &tempAdditionalOffsetFirst, 0.002f);
                                tempSliderUI->SetAdditionalOffsetFirst(tempAdditionalOffsetFirst);

                                float tempAdditionalOffsetLast = tempSliderUI->GetAdditionalOffsetLast();
                                ImGui::DragFloat("Slider Last Additional Offset", &tempAdditionalOffsetLast, 0.002f);
                                tempSliderUI->SetAdditionalOffsetLast(tempAdditionalOffsetLast);

                                if (ImGui::ArrowButton("Change Section to edit Left 2", 0))
                                {
                                    sliderDesignOptionToModify--;
                                    if (sliderDesignOptionToModify < 0)
                                    {
                                        sliderActivePart = !sliderActivePart;
                                        sliderDesignOptionToModify = 2;
                                    }
                                }
                                ImGui::SameLine();
                                switch (sliderDesignOptionToModify)
                                {
                                case 0:
                                    ImGui::Text(sliderActivePart ? "  Middle Part Activated  " : "  Middle Part Disabled  ");
                                    break;
                                case 1:
                                    ImGui::Text(sliderActivePart ? "  First Part Activated  " : "  First Part Disabled  ");
                                    break;
                                case 2:
                                    ImGui::Text(sliderActivePart ? "  Last Part Activated  " : "  Last Part Disabled  ");
                                    break;
                                default:
                                    break;
                                }
                                ImGui::SameLine();
                                if (ImGui::ArrowButton("Change Section to edit Right 2", 1))
                                {
                                    sliderDesignOptionToModify++;
                                    if (sliderDesignOptionToModify >= 3)
                                    {
                                        sliderActivePart = !sliderActivePart;
                                        sliderDesignOptionToModify = 0;
                                    }
                                }

                                if (ImGui::CollapsingHeader("Image section IDLE info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as idle"))
                                    {
                                        tempSliderUI->SetState(UiState::IDLE);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectIdle(sliderDesignOptionToModify, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectIdle(sliderDesignOptionToModify, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectIdle(sliderDesignOptionToModify, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectIdle(sliderDesignOptionToModify, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2IdleSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##SliderIdleSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2IdleSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2IdleSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::IDLE)
                                    {
                                        tempSliderUI->SetSectSizeIdle(tempX2, tempY2, tempW2, tempH2, sliderDesignOptionToModify, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                                if (ImGui::CollapsingHeader("Image section HOVERED info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as hovered"))
                                    {
                                        tempSliderUI->SetState(UiState::HOVERED);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectHovered(sliderDesignOptionToModify, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectHovered(sliderDesignOptionToModify, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectHovered(sliderDesignOptionToModify, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectHovered(sliderDesignOptionToModify, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2HoveredSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2HoveredSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2HoveredSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2HoveredSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::HOVERED)
                                    {
                                        tempSliderUI->SetSectSizeHovered(tempX2, tempY2, tempW2, tempH2, sliderDesignOptionToModify, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                                if (ImGui::CollapsingHeader("Image section SELECTED info: ", treeNodeFlags))
                                {
                                    if (ImGui::Button("Set current Section ptr as selected"))
                                    {
                                        tempSliderUI->SetState(UiState::SELECTED);
                                    }
                                    float tempX2, tempY2, tempW2, tempH2;
                                    tempX2 = tempSliderUI->GetSectSelected(sliderDesignOptionToModify, sliderActivePart).x;
                                    tempY2 = tempSliderUI->GetSectSelected(sliderDesignOptionToModify, sliderActivePart).y;
                                    tempW2 = tempSliderUI->GetSectSelected(sliderDesignOptionToModify, sliderActivePart).w;
                                    tempH2 = tempSliderUI->GetSectSelected(sliderDesignOptionToModify, sliderActivePart).h;
                                    ImGui::Text("   X:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2SelectedSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                    ImGui::Text("   Y:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2SelectedSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                    ImGui::Text("   W:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2SelectedSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                    ImGui::Text("   H:");
                                    ImGui::SameLine();
                                    ImGui::DragFloat(("##Slider2SelectedSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                    if (tempSliderUI->GetState() == UiState::SELECTED)
                                    {
                                        tempSliderUI->SetSectSizeSelected(tempX2, tempY2, tempW2, tempH2, sliderDesignOptionToModify, sliderActivePart);
                                    }
                                    ImGui::Dummy(ImVec2(0.0f, 5.0f));
                                }
                            }
                        }
                        else if (item->GetType() == UiType::CHECKER)
                        {
                            CheckerUI* tempCheckerUI = tempCanvas->GetItemUI<CheckerUI>(id);
                            ImGui::Text("UiType: CHECKER");
                            
                            bool checkerActive = tempCheckerUI->GetChecker();
                            ImGui::Checkbox(("Toggle Checkbox State##" + std::to_string(id)).c_str(), &checkerActive);
                            tempCheckerUI->SetChecker(checkerActive);

                            ImGui::Text("Image Path: %s", tempCheckerUI->GetPath().c_str());
                            if (ImGui::CollapsingHeader("Image section IDLE info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as idle"))
                                {
                                    tempCheckerUI->SetState(UiState::IDLE);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempCheckerUI->GetSectIdle().x;
                                tempY2 = tempCheckerUI->GetSectIdle().y;
                                tempW2 = tempCheckerUI->GetSectIdle().w;
                                tempH2 = tempCheckerUI->GetSectIdle().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerIdleSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerIdleSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerIdleSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerIdleSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempCheckerUI->GetState() == UiState::IDLE)
                                {
                                    tempCheckerUI->SetSectSizeIdle(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                            if (ImGui::CollapsingHeader("Image section HOVERED info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as hovered"))
                                {
                                    tempCheckerUI->SetState(UiState::HOVERED);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempCheckerUI->GetSectHovered().x;
                                tempY2 = tempCheckerUI->GetSectHovered().y;
                                tempW2 = tempCheckerUI->GetSectHovered().w;
                                tempH2 = tempCheckerUI->GetSectHovered().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerHoveredSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerHoveredSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerHoveredSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerHoveredSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempCheckerUI->GetState() == UiState::HOVERED)
                                {
                                    tempCheckerUI->SetSectSizeHovered(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                            if (ImGui::CollapsingHeader("Image section SELECTED info: ", treeNodeFlags))
                            {
                                if (ImGui::Button("Set current Section ptr as selected"))
                                {
                                    tempCheckerUI->SetState(UiState::SELECTED);
                                }
                                float tempX2, tempY2, tempW2, tempH2;
                                tempX2 = tempCheckerUI->GetSectSelected().x;
                                tempY2 = tempCheckerUI->GetSectSelected().y;
                                tempW2 = tempCheckerUI->GetSectSelected().w;
                                tempH2 = tempCheckerUI->GetSectSelected().h;
                                ImGui::Text("   X:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerSelectedSectionX of UIid" + std::to_string(item->GetID())).c_str(), &tempX2, 1.0f);
                                ImGui::Text("   Y:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerSelectedSectionY of UIid" + std::to_string(item->GetID())).c_str(), &tempY2, 1.0f);
                                ImGui::Text("   W:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerSelectedSectionW of UIid" + std::to_string(item->GetID())).c_str(), &tempW2, 1.0f);
                                ImGui::Text("   H:");
                                ImGui::SameLine();
                                ImGui::DragFloat(("##CheckerSelectedSectionH of UIid" + std::to_string(item->GetID())).c_str(), &tempH2, 1.0f);

                                if (tempCheckerUI->GetState() == UiState::SELECTED)
                                {
                                    tempCheckerUI->SetSectSizeSelected(tempX2, tempY2, tempW2, tempH2);
                                }
                                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                            }
                        }
                        else if (item->GetType() == UiType::TEXT)
                        {
                            TextUI* tempTextUI = tempCanvas->GetItemUI<TextUI>(id);
                            ImGui::Text("UiType: TEXT");
                            ImGui::Text("Font Path: %s", tempTextUI->GetPath().c_str());
                            if (ImGui::TreeNode(("Change Font##" + std::to_string(item->GetID())).c_str()))
                            {
                                std::vector<Resources::Resource*> tempFontResources = Resources::GetResourcesOf(Resources::RES_FONT);

                                for (auto& item : tempFontResources)
                                {
                                    if (ImGui::MenuItem(item->name.c_str()))
                                    {
                                        tempTextUI->SetFont(item->filePath.c_str());
                                    }
                                }
                                if (ImGui::MenuItem("Import Font"))
                                {
                                    std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Font file (*.ttf)\0*.ttf\0")).string();
                                    if (!filePath.empty() && filePath.ends_with(".ttf"))
                                    {
                                        tempTextUI->SetFont(filePath.c_str());
                                    }
                                }
                                ImGui::TreePop();
                            }
                            ImGui::Text("Text info:");
                            static char currentTextString[512]; // Buffer para el nuevo nombre
                            ImGui::InputTextMultiline(("##Text String chupala ticher" + std::to_string(item->GetID())).c_str(), currentTextString, sizeof(currentTextString));
                            if (ImGui::Button("Set Text"))
                            {
                                std::string newName(currentTextString);
                                tempTextUI->SetText(newName);
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Recover Text"))
                            {
                                strcpy(currentTextString, tempTextUI->GetText().c_str());
                            }
                            float r, g, b, a;
                            r = tempTextUI->GetColor().r;
                            g = tempTextUI->GetColor().g;
                            b = tempTextUI->GetColor().b;
                            a = tempTextUI->GetColor().a;
                            ImGui::Text("   R:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text Color R" + std::to_string(item->GetID())).c_str(), &r, 0.005f, 0.0f, 1.0f);
                            ImGui::Text("   G:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text Color G" + std::to_string(item->GetID())).c_str(), &g, 0.005f, 0.0f, 1.0f);
                            ImGui::Text("   B:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text Color B" + std::to_string(item->GetID())).c_str(), &b, 0.005f, 0.0f, 1.0f);
                            ImGui::Text("   A:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text Color A" + std::to_string(item->GetID())).c_str(), &a, 0.005f, 0.0f, 1.0f);

                            tempTextUI->SetColor({ r,g,b,a });

                            float k, l;
                            k = tempTextUI->GetKerning();
                            l = tempTextUI->GetLineSpacing();

                            ImGui::Text("   Kerning:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text Kerning" + std::to_string(item->GetID())).c_str(), &k, 0.005f, -0.1f, 1.0f);
                            tempTextUI->SetKerning(k);

                            ImGui::Text("   LineSpacing:");
                            ImGui::SameLine();
                            ImGui::DragFloat(("##Text LineSpacing" + std::to_string(item->GetID())).c_str(), &l, 0.005f, -0.3f, 1.0f);
                            tempTextUI->SetLineSpacing(l);
                        }
                        else if (item->GetType() == UiType::UNKNOWN)
                        {
                            ImGui::Text("UiType: UNKNOWN TYPE");
                        }

                        ImGui::Dummy(ImVec2(0.0f, 5.0f));
                        if (ImGui::Button("Remove ItemUI"))
                        {
                            tempCanvas->RemoveItemUI(id);
                        }
                        ImGui::Dummy(ImVec2(0.0f, 5.0f));
                        if (ImGui::Button("Duplicate ItemUI"))
                        {
                            switch (item->GetType())
                            {
                            case UiType::IMAGE:
                                tempCanvas->AddCopiedItemUI<ImageUI>((ImageUI*)item);
                                break;
                            case UiType::BUTTONIMAGE:
                                tempCanvas->AddCopiedItemUI<ButtonImageUI>((ButtonImageUI*)item);
                                break;
                            case UiType::SLIDER:
                                tempCanvas->AddCopiedItemUI<SliderUI>((SliderUI*)item);
                                break;
                            case UiType::CHECKER:
                                tempCanvas->AddCopiedItemUI<CheckerUI>((CheckerUI*)item);
                                break;
                            case UiType::TEXT:
                                tempCanvas->AddCopiedItemUI<TextUI>((TextUI*)item);
                                break;
                            case UiType::UNKNOWN:
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    counter++;
                }

                //adders and removers of itemui
                ImGui::Dummy(ImVec2(0.0f, 5.0f));
                if (ImGui::TreeNode("Add ItemUI"))
                {
                    if (ImGui::TreeNode("ImageUI"))
                    {
                        if (ImGui::MenuItem("Import Image"))
                        {
                            std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Image file (*.png)\0*.png\0")).string();
                            if (!filePath.empty() && filePath.ends_with(".png"))
                            {
                                tempCanvas->AddItemUI<ImageUI>(filePath.c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("ButtonImageUI"))
                    {
                        if (ImGui::MenuItem("Import Image"))
                        {
                            std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Image file (*.png)\0*.png\0")).string();
                            if (!filePath.empty() && filePath.ends_with(".png"))
                            {
                                tempCanvas->AddItemUI<ButtonImageUI>(filePath.c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("SliderUI"))
                    {
                        if (ImGui::MenuItem("Import Image"))
                        {
                            std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Image file (*.png)\0*.png\0")).string();
                            if (!filePath.empty() && filePath.ends_with(".png"))
                            {
                                tempCanvas->AddItemUI<SliderUI>(filePath.c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("CheckerUI"))
                    {
                        if (ImGui::MenuItem("Import Image"))
                        {
                            std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Image file (*.png)\0*.png\0")).string();
                            if (!filePath.empty() && filePath.ends_with(".png"))
                            {
                                tempCanvas->AddItemUI<CheckerUI>(filePath.c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("TextUI"))
                    {
                        std::vector<Resources::Resource*> tempFontResources = Resources::GetResourcesOf(Resources::RES_FONT);

                        for (auto& item : tempFontResources)
                        {
                            if (ImGui::MenuItem(item->name.c_str()))
                            {
                                tempCanvas->AddItemUI<TextUI>(item->filePath.c_str());
                            }
                        }
                        if (ImGui::MenuItem("Import Font"))
                        {
                            std::string filePath = std::filesystem::relative(FileDialog::OpenFile("Open Font file (*.ttf)\0*.ttf\0")).string();
                            if (!filePath.empty() && filePath.ends_with(".ttf"))
                            {
                                tempCanvas->AddItemUI<TextUI>(filePath.c_str());
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }


                //remover of canvas
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::Button("Remove Canvas"))
                {
                    selectedGO->RemoveComponent(ComponentType::Canvas);
                }
            }


            /*Listener Component*/
            Listener* listener = selectedGO->GetComponent<Listener>();

            if (listener != nullptr && ImGui::CollapsingHeader("Listener", treeNodeFlags))
            {
                // No properties
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::Button("Remove Listener"))
                {
                    audioManager->audio->RemoveDefaultListener(selectedGO->GetComponent<Listener>()->goID);
                    audioManager->audio->UnregisterGameObject(selectedGO->GetComponent<Listener>()->goID, selectedGO->GetComponent<Listener>()->GetName());

                    selectedGO->RemoveComponent(ComponentType::Listener);
                }
            }

            /*AudioSource Component*/
            AudioSource* audioSource = selectedGO->GetComponent<AudioSource>();

            if (audioSource != nullptr && ImGui::CollapsingHeader("Audio Source", ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_DefaultOpen)) {
                //remover of Audio Source
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::Button("Remove Audio Source"))
                {
                    audioManager->audio->UnregisterGameObject(selectedGO->GetComponent<AudioSource>()->goID, selectedGO->GetComponent<AudioSource>()->GetName());

                    selectedGO->RemoveComponent(ComponentType::AudioSource);
                }
            }

            /*Add Component*/
            if (ImGui::BeginMenu("Add Component"))
            {
                if (ImGui::MenuItem("New Script"))
                    chooseScriptNameWindow = true;

                if (ImGui::MenuItem("Particle System"))
                {
                    selectedGO->AddComponent<ParticleSystem>();
                }
                
                if (ImGui::MenuItem("Light"))
                {
                    selectedGO->AddComponent<Light>();
                }
                
                if (ImGui::MenuItem("Listener"))
                {
                    selectedGO->AddComponent<Listener>();
                    selectedGO->GetComponent<Listener>()->goID = audioManager->audio->RegisterGameObject(selectedGO->GetName());
                    audioManager->AddAudioObject((std::shared_ptr<AudioComponent>)selectedGO->GetComponent<Listener>());
                    audioManager->audio->SetDefaultListener(selectedGO->GetComponent<Listener>()->goID);
                }

                if (ImGui::MenuItem("AudioSource"))
                {
                    selectedGO->AddComponent<AudioSource>();
                    selectedGO->GetComponent<AudioSource>()->goID = audioManager->audio->RegisterGameObject(selectedGO->GetName());
                    audioManager->AddAudioObject((std::shared_ptr<AudioComponent>)selectedGO->GetComponent<AudioSource>());
                }

                if (ImGui::TreeNode("Collider2D"))
                {
                    if (ImGui::TreeNode("Player"))
                    {
                        if (ImGui::MenuItem("Circle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Circle;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Player;
                        }
                        if (ImGui::MenuItem("Rectangle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Rect;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Player;
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Enemy"))
                    {
                        if (ImGui::MenuItem("Circle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Circle;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Enemy;
                        }
                        if (ImGui::MenuItem("Rectangle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Rect;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Enemy;
                        }
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Wall"))
                    {
                        if (ImGui::MenuItem("Circle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Circle;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Wall;
                        }
                        if (ImGui::MenuItem("Rectangle"))
                        {
                            selectedGO->AddComponent<Collider2D>();
                            selectedGO->GetComponent<Collider2D>()->colliderType = ColliderType::Rect;
                            selectedGO->GetComponent<Collider2D>()->collisionType = CollisionType::Wall;
                        }
                        ImGui::TreePop();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::MenuItem("New Canvas"))
                {
                    selectedGO->AddComponent<Canvas>();
                }

                /*ImGuiTextFilter filter;
                filter.Draw();*/
                /*if (ImGui::Selectable("Script"))
                {
                    chooseScriptNameWindow = true;
                }*/
                /*for (int i = 0; i < IM_ARRAYSIZE(names); i++)
                    if (ImGui::Selectable(names[i]))
                        selected_fish = i;
                ImGui::EndPopup();*/

                ImGui::EndMenu();
            }

            //if (ImGui::BeginPopup("Select New Component"))
            //{
            //    /*ImGuiTextFilter filter;
            //    filter.Draw();*/
            //    ImGui::SeparatorText("Components");
            //    if (ImGui::Selectable("Script"))
            //    {
            //        chooseScriptNameWindow = true;
            //    }
            //    /*for (int i = 0; i < IM_ARRAYSIZE(names); i++)
            //        if (ImGui::Selectable(names[i]))
            //            selected_fish = i;
            //    ImGui::EndPopup();*/
            //}
        }

        if(chooseScriptNameWindow) ChooseScriptNameWindow();
        else if (chooseParticlesToImportWindow) ChooseParticlesToImportWindow();

	}
    ImGui::End();

	return true;
}

void PanelInspector::ChooseScriptNameWindow()
{
    ImGui::Begin("Script name", &chooseScriptNameWindow);

    static char nameRecipient[32];

    ImGui::InputText("File Name", nameRecipient, IM_ARRAYSIZE(nameRecipient));

    if (engine->inputManager->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && nameRecipient != "")
    {
        //std::string className = "ActualScriptTest2";
        if (MonoManager::IsClassInMainAssembly(nameRecipient))
        {
            selectedGO->AddComponent<Script>(nameRecipient);
            ImGui::CloseCurrentPopup();
        }
        else
        {
           LOG(LogType::LOG_WARNING, "Could not find class '%s'", nameRecipient);
        }

        chooseScriptNameWindow = false;
    }

    ImGui::End();
}

void PanelInspector::ChooseParticlesToImportWindow()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Particles name", &chooseParticlesToImportWindow);

    static char nameRecipient[32];

    ImGui::InputText("File Name", nameRecipient, IM_ARRAYSIZE(nameRecipient));

    std::string nameFile = nameRecipient;

    nameFile = nameFile + ".particles";

    fs::path assetsDir = fs::path(ASSETS_PATH) / "Particles" / nameFile;

    if (engine->inputManager->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && nameRecipient != "")
    {
        //std::string className = "ActualScriptTest2";
        if (std::filesystem::exists(assetsDir))
        {
            // Read the scene JSON from the file
            std::ifstream file(assetsDir);
            if (!file.is_open())
            {
                LOG(LogType::LOG_ERROR, "Failed to open scene file: {}", assetsDir);
                return;
            }

            json particlesJSON;

            try
            {
                file >> particlesJSON;
            }
            catch (const json::parse_error& e)
            {
                LOG(LogType::LOG_ERROR, "Failed to parse scene JSON: {}", e.what());
                return;
            }

            // Close the file
            file.close();

            selectedGO->GetComponent<ParticleSystem>()->LoadComponent(particlesJSON);
            ImGui::CloseCurrentPopup();
        }
        else
        {
            LOG(LogType::LOG_WARNING, "Could not find file '%s'", nameFile);
        }

        chooseParticlesToImportWindow = false;
    }

    ImGui::End();
}

void PanelInspector::OverridePrefabs(GameObject& gameObject)
{
    OverridePrefabFile(gameObject);
    engine->N_sceneManager->OverrideScenePrefabs(gameObject.GetPrefabID());
}

void PanelInspector::OverridePrefabFile(GameObject& gameObject)
{
    //gameObject.SetPrefab(UIDGen::GenerateUID());

    // Serialize the GameObject and save it as a prefab file
    json gameObjectJSON = gameObject.SaveGameObject();

    fs::path filename = ASSETS_PATH;
    filename += "Prefabs\\" + gameObject.GetName() + ".prefab";

    std::ofstream(filename) << gameObjectJSON.dump(2);

    LOG(LogType::LOG_OK, "PREFAB UPDATED SUCCESSFULLY");

    gameObject.SetPrefabDirty(false);
}
