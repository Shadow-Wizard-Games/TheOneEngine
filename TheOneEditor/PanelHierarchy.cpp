#include "PanelHierarchy.h"
#include "App.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Gui.h"
#include "imgui.h"
#include "..\TheOneEngine\EngineCore.h"

#include <variant>

PanelHierarchy::PanelHierarchy(PanelType type, std::string name) : Panel(type, name), open_selected(false), reparent(false) {}

PanelHierarchy::~PanelHierarchy() {}

void PanelHierarchy::RecurseShowChildren(std::shared_ptr<GameObject> parent)
{
	for (const auto& childGO : parent.get()->children)
	{
		uint treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (childGO.get()->children.size() == 0)
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		if (childGO == engine->N_sceneManager->GetSelectedGO())
			treeFlags |= ImGuiTreeNodeFlags_Selected;

		//bool isOpen = false;
		//if (!childGO.get()->GetName().empty())
		//{
		if (childGO.get()->IsPrefab() && childGO.get()->IsEditablePrefab())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.2f, .8f, 1.f, 1.f));
		}
		else if (childGO.get()->IsPrefab() && !childGO.get()->IsEditablePrefab())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .3f, .2f, 1.f));
		}

		bool isOpen = ImGui::TreeNodeEx(childGO.get()->GetName().data(), treeFlags);
		//}

		if (ReparentDragDrop(childGO))
			break;

		if (ImGui::IsItemClicked(0) && !ImGui::IsItemToggledOpen())
		{
			engine->N_sceneManager->SetSelectedGO(childGO);
			LOG(LogType::LOG_INFO, "SelectedGO: %s", engine->N_sceneManager->GetSelectedGO().get()->GetName().c_str());
		}

		ContextMenu(childGO);
		if (duplicate)
		{
			duplicate = false;
			break;
		}
		if (createEmpty)
		{
			createEmpty = false;
			break;
		}
		if (remove)
		{
			break;
		}
		if (isOpen && !reparent)
		{
			//treeFlags &= ~ImGuiTreeNodeFlags_Selected;
			RecurseShowChildren(childGO);

			ImGui::TreePop();
		}
		ImGui::PopStyleColor(3);
	}

	if (remove)
	{
		for (auto& item : toDeleteList)
		{
			item.get()->Delete();
		}

		remove = false;
	}
}

bool PanelHierarchy::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	reparent = false;

	uint treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (ImGui::Begin(name.c_str(), &enabled, settingsFlags))
	{
		if (ImGui::TreeNodeEx(engine->N_sceneManager->currentScene->GetRootSceneGO().get()->GetName().data(), treeFlags))
		{
			reparent = false;
			ReparentDragDrop(engine->N_sceneManager->currentScene->GetRootSceneGO());
			RecurseShowChildren(engine->N_sceneManager->currentScene->GetRootSceneGO());
			ImGui::TreePop();
		}

		ImGui::End();
	}

	return true;
}

void PanelHierarchy::ContextMenu(std::shared_ptr<GameObject> go)
{
	if (ImGui::BeginPopupContextItem())
	{
		if (go.get()->IsEditablePrefab() && ImGui::MenuItem("Create Empty"))
		{
			createEmpty = true;
			engine->N_sceneManager->ReparentGO(go, engine->N_sceneManager->CreateEmptyGO("Parent of " + go.get()->GetName(), false));
			LOG(LogType::LOG_INFO, "Empty parent for %s created", go.get()->GetName().c_str());
		}

		if (ImGui::MenuItem("Duplicate"))
		{
			duplicate = true;
			engine->N_sceneManager->SetSelectedGO(engine->N_sceneManager->DuplicateGO(go));
			LOG(LogType::LOG_INFO, "%s has been duplicated", go.get()->GetName().c_str());
		}

		if (ImGui::MenuItem("Remove"))
		{
			remove = true;
			LOG(LogType::LOG_INFO, "Use Count: %d", go.use_count());
			engine->N_sceneManager->SetSelectedGO(nullptr);

			toDeleteList.push_back(go);

			//go.get()->Delete();
			//go.get()->Disable();
		}
		
		if ((go.get()->IsPrefab() && go.get()->IsEditablePrefab()) && ImGui::MenuItem("Lock"))
		{
			go.get()->SetEditablePrefab(false);
		}

		if ((go.get()->IsPrefab() && !go.get()->IsEditablePrefab()) && ImGui::MenuItem("Unlock"))
		{
			go.get()->SetEditablePrefab(true);
		}

		ImGui::EndPopup();
	}
}

bool PanelHierarchy::ReparentDragDrop(std::shared_ptr<GameObject> childGO)
{
	if (ImGui::BeginDragDropSource())
	{
		if (childGO != engine->N_sceneManager->currentScene->GetRootSceneGO())
		{
			if (childGO.get()->IsEditablePrefab())
			{
				ImGui::SetDragDropPayload(engine->N_sceneManager->GetSelectedGO().get()->GetName().c_str(), &childGO, sizeof(GameObject));
			}
		}

		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		if (engine->N_sceneManager->GetSelectedGO().get())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(engine->N_sceneManager->GetSelectedGO().get()->GetName().c_str()))
			{
				GameObject* dragging = *(GameObject**)payload->Data;

				GameObject* currentParent = childGO.get();

				if (childGO.get()->IsEditablePrefab())
				{
					while (currentParent)
					{
						if (currentParent == dragging)
						{
							return false;
						}
						currentParent = currentParent->parent.lock().get();
					}

					GameObject* oldParent = dragging->parent.lock().get();

					dragging->parent = childGO.get()->weak_from_this();

					std::shared_ptr<GameObject> newChild = dragging->weak_from_this().lock();

					if (oldParent != nullptr)
					{
						std::vector<std::shared_ptr<GameObject>>::iterator position = std::find(oldParent->children.begin(), oldParent->children.end(), newChild);
						oldParent->children.erase(position);
					}

					if (dragging->parent.lock().get()->IsPrefab())
					{
						dragging->SetPrefab(dragging->parent.lock().get()->GetPrefabID());
					}

					childGO.get()->children.emplace_back(newChild);
					reparent = true;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return reparent;
}