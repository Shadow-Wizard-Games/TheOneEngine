#include "PanelInspector.h"
#include "SceneManager.h"
#include "App.h"
#include "Gui.h"
#include "SceneManager.h"

#include "..\TheOneEngine\Log.h"
#include "..\TheOneEngine\GameObject.h"
#include "..\TheOneEngine\Transform.h"
#include "..\TheOneEngine\Mesh.h"
#include "..\TheOneEngine\Camera.h"
#include "..\TheOneEngine\Script.h"
#include "..\TheOneEngine\Collider2D.h"
#include "..\TheOneEngine\MonoManager.h"

#include "../TheOneAudio/AudioCore.h"

#include "imgui.h"
#include "imgui_internal.h"

PanelInspector::PanelInspector(PanelType type, std::string name) : Panel(type, name)
{
    matrixDirty = false;
    chooseScriptNameWindow = false;
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
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            /*Tag + Layer*/
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Tag", "Untagged", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }

            ImGui::SameLine();

            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::BeginCombo("Layer", "Default", ImGuiComboFlags_HeightSmall)) { ImGui::EndCombo(); }
            ImGui::Dummy(ImVec2(0.0f, 10.0f));

            //add change name imgui
            static char newNameBuffer[256]; // Buffer para el nuevo nombre
            if (ImGui::InputText("New Name", newNameBuffer, sizeof(newNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string newName(newNameBuffer);
                LOG(LogType::LOG_INFO, "GameObject %s has been renamed to %s", selectedGO->GetName().c_str(), newName.c_str());
                selectedGO->SetName(newName); // Establece el nuevo nombre del GameObject
                // Limpiar el buffer después de cambiar el nombre
                newNameBuffer[0] = '\0';
            }

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
                ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
                //ImGui::Indent(0.8f);
                if (ImGui::BeginTable("", 4, tableFlags))
                {
                    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);

                    ImGui::TableNextRow();

                    // Headers
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TableHeader("");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TableHeader("X");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TableHeader("Y");
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TableHeader("Z");

                    ImGui::TableNextRow();

                    // Position
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Position");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##PosX", &view_pos.x, 0.5F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##PosY", &view_pos.y, 0.5F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##PosZ", &view_pos.z, 0.5F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableNextRow();

                    // Rotation
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Rotation");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##RotX", &view_rot_deg.x, 0.2f, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##RotY", &view_rot_deg.y, 0.2f, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##RotZ", &view_rot_deg.z, 0.2f, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableNextRow();

                    // Scale
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Scale");

                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::DragFloat("##ScaleX", &view_sca.x, 0.1F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::DragFloat("##ScaleY", &view_sca.y, 0.1F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::DragFloat("##ScaleZ", &view_sca.z, 0.1F, 0, 0, "%.3f", 1)) matrixDirty = true;

                    ImGui::EndTable();
                }

                if (matrixDirty)
                {
                    // translate > rotate > scale

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
                ImGui::SameLine();  ImGui::TextColored({ 0.920f, 0.845f, 0.0184f, 1.0f }, mesh->mesh.meshName.c_str());
                ImGui::Separator();
                ImGui::Text("Indexes: ");
                ImGui::SameLine();  ImGui::Text((std::to_string(mesh->mesh.indexs_buffer_id)).c_str());
                ImGui::Text("Vertexs: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numVerts).c_str());
                ImGui::Text("Faces: ");
                ImGui::SameLine();  ImGui::Text(std::to_string(mesh->mesh.numFaces).c_str());

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

                if (ImGui::BeginCombo("Camera Type", camera->cameraType == CameraType::PERSPECTIVE ? "Perspective" : "Orthogonal"))
                {
                    if (ImGui::Selectable("Perspective", camera->cameraType == CameraType::PERSPECTIVE))
                    {
                        camera->cameraType = CameraType::PERSPECTIVE;
                        LOG(LogType::LOG_INFO, "Camera projection changed to PERSPECTIVE");
                        isDirty = true;
                    }

                    if (ImGui::Selectable("Orthogonal", camera->cameraType == CameraType::ORTHOGONAL))
                    {
                        camera->cameraType = CameraType::ORTHOGONAL;
                        LOG(LogType::LOG_INFO, "Camera projection changed to ORTHOGONAL");
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
                
                if (camera->cameraType == CameraType::ORTHOGONAL)
                {
                    if (ImGui::SliderFloat("SIZE", &size, 0.1, 100.0))
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
                
                if (isDirty) camera->UpdateCamera();


                ImGui::Checkbox("Draw Frustrum", &camera->drawFrustum);

                if (ImGui::Checkbox("Primary Camera", &camera->primaryCam))
                {
                    engine->N_sceneManager->currentScene->ChangePrimaryCamera(selectedGO);
                }

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }
            

            /*Script Component*/
            Script* script = selectedGO->GetComponent<Script>();

            if (script != nullptr && ImGui::CollapsingHeader("Script", treeNodeFlags))
            {
                
                ImGui::Dummy(ImVec2(0.0f, 10.0f));
            }


            /*Collider2D Component*/
            Collider2D* collider2D = selectedGO->GetComponent<Collider2D>();

            if (collider2D != nullptr && ImGui::CollapsingHeader("Collider 2D", treeNodeFlags))
            {
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

                if (collider2D->colliderType == ColliderType::Rect)
                {
                    // Rectangle collider
                    float w = (float)collider2D->w;
                    float h = (float)collider2D->h;
                    ImGui::Text("Width");
                    ImGui::SameLine();
                    if (ImGui::InputFloat("##Width", &w))
                    {
                        collider2D->w = w;
                    }

                    ImGui::Text("Height");
                    ImGui::SameLine();
                    if (ImGui::InputFloat("##Height", &h))
                    {
                        collider2D->h = h;
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
                    }
                }

                ImGui::Dummy(ImVec2(0.0f, 10.0f));
                if (ImGui::Button("Remove Collider"))
                {
                    selectedGO->RemoveComponent(ComponentType::Collider2D);
                }
            }

            /*Add Component*/
            if (ImGui::BeginMenu("Add Component"))
            {
                if (ImGui::MenuItem("New Script"))
                    chooseScriptNameWindow = true;

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

        ImGui::End();
	}	

    ImGui::PopStyleVar();

	return true;
}

void PanelInspector::ChooseScriptNameWindow()
{
    ImGui::Begin("Script name", &chooseScriptNameWindow);

    static char nameRecipient[32];

    ImGui::InputText("File Name", nameRecipient, IM_ARRAYSIZE(nameRecipient));

    if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && nameRecipient != "")
    {
        //std::string className = "ActualScriptTest2";
        if (MonoManager::IsClassInMainAssembly(nameRecipient))
        {
            selectedGO->AddScript(nameRecipient);
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