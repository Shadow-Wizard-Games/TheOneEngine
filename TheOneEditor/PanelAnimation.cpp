#include "PanelAnimation.h"

#include "App.h"
#include "Gui.h"

#include "../TheOneEngine/EngineCore.h"
#include "../TheOneEngine/N_SceneManager.h"
#include "../TheOneEngine/FrameBuffer.h"
#include "../TheOneEngine/FileDialog.h"
#include "../TheOneEngine/Camera.h"
#include "../TheOneEngine/Model.h"
#include "../TheOneEngine/Animation/animations/OzzAnimationSimple.h"
#include "../TheOneEngine/Animation/animations/OzzAnimationPartialBlending.h"

#include "imgui.h"
#include "imgui_stdlib.h"


// hekbas: TODO OZZ
PanelAnimation::PanelAnimation(PanelType type, std::string name) : Panel(type, name),
activeAnimator(nullptr),
animationCamera(nullptr),
isPlaying(false)
{
	frameBuffer = std::make_shared<FrameBuffer>(1280, 720, true);

	animationCamera = std::make_shared<GameObject>("ANIMATION CAMERA");
	animationCamera.get()->AddComponent<Transform>();
	animationCamera.get()->GetComponent<Transform>()->SetPosition(vec3f(0, 3, -10));
	animationCamera.get()->AddComponent<Camera>();
	animationCamera.get()->GetComponent<Camera>()->UpdateCamera();
}

PanelAnimation::~PanelAnimation() {}

bool PanelAnimation::Draw()
{
	if (ImGui::Begin("Animation", &enabled))
	{
		if (!AnimationAvaliable())
		{
			ImGui::Text("Select a GameObject with a Skeleton");
			ImGui::End();
			return false;
		}

		if (ImGui::BeginChild("Settings", { 400, 0 }, false, ImGuiTableFlags_Resizable))
		{
			Settings();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("Viewport", { 0, 0 }, false, ImGuiTableFlags_Resizable))
		{
			Viewport();
		}
		ImGui::EndChild();		
	}
	ImGui::End();

	return true;
}

bool PanelAnimation::AnimationAvaliable()
{
	GameObject* selectedGO = engine->N_sceneManager->GetSelectedGO().get();

	if (!selectedGO)
	{
		return false;
	}

	if (!selectedGO->GetComponent<Mesh>())
	{
		return false;
	}

	auto resourceID = selectedGO->GetComponent<Mesh>()->meshID;

	if (!Resources::GetResourceById<Model>(resourceID)->isAnimated())
	{
		return false;
	}

	activeAnimator = Resources::GetResourceById<Model>(resourceID);

	if(isPlaying)
		activeAnimator->UpdateAnim(app->GetDT());

	return true;
}

void PanelAnimation::Settings()
{
	ImGui::Text("Animator file: %s", activeAnimator->GetMeshPath().c_str());

	if (ImGui::Button("Import FBX Animation"))
	{
		std::string filePath = FileDialog::OpenFile("Open FBX with animation (*.fbx)\0*.fbx\0");
		if (!filePath.empty() && filePath.ends_with(".fbx"))
		{
			activeAnimator->ImportAnimation(filePath.c_str());
		}
	}

	if (ImGui::Button("Save animator") && activeAnimator)
		activeAnimator->SaveAnimator();

	bool blend = activeAnimator->getBlendOnTransition();

	if (ImGui::Checkbox("Blend on transition", &blend))
		activeAnimator->setBlendOnTransition(blend);

	float transtime = activeAnimator->getTransitionTime();

	if (blend)
	{
		if (ImGui::InputFloat("Transition time", &transtime))
			activeAnimator->setTransitionTime(transtime);
	}
	else
	{
		ImGui::BeginDisabled();
		ImGui::InputFloat("Transition time", &transtime);
		ImGui::EndDisabled();
	}

	DrawAnimations();
}

// hekbas: TODO OZZ
void PanelAnimation::Viewport()
{
	// Set viewport size
	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	if (viewportSize.x > 0.0f && viewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(frameBuffer->getWidth() != viewportSize.x || frameBuffer->getHeight() != viewportSize.y))
	{
		frameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		animationCamera.get()->GetComponent<Camera>()->aspect = viewportSize.x / viewportSize.y;
	}

	//ALL DRAWING MUST HAPPEN BETWEEN FB BIND/UNBIND
	{
		frameBuffer->Bind();
		frameBuffer->Clear({ 0.13f, 0.14f, 0.15f, 1.00f });
		frameBuffer->ClearBuffer(-1);

		// Draw
		engine->N_sceneManager->GetSelectedGO().get()->GetComponent<Mesh>()->DrawComponent(animationCamera->GetComponent<Camera>());

		/*current->Draw(DrawMode::EDITOR, animationCamera->GetComponent<Camera>());
		if (engine->N_sceneManager->GetSceneIsChanging())
			engine->N_sceneManager->loadingScreen->DrawUI(engine->N_sceneManager->currentScene->currentCamera, DrawMode::GAME);*/

		frameBuffer->Unbind();
	}

	//Draw FrameBuffer Texture
	ImGui::Image((ImTextureID)frameBuffer->getColorBufferTexture(), { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });
}

void PanelAnimation::DrawAnimations()
{
	// Add animation button
	if (ImGui::Button("Add animation")) {
		ImGui::OpenPopup("add_anim");
	}

	// Add animation popup
	if (ImGui::BeginPopup("add_anim"))
	{
		ImGui::Text("Add animation");

		static std::string animation_name;

		ImGui::InputText("Name", &animation_name);

		const char* items[] = { "Simple animation", "Partial blending"};

		static int current_item = 0;

		if (ImGui::BeginCombo("anim_type", items[current_item]))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (current_item == n); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected))
				{
					current_item = n;

					// You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					if (is_selected) ImGui::SetItemDefaultFocus();   
				}
			}

			ImGui::EndCombo();
		}

		static std::string error_msg;

		if (error_msg != "") {
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), error_msg.c_str());
		}

		if (ImGui::Button("Add"))
		{
			size_t anim_id = 0;

			switch (current_item)
			{
				case AnimationType::AT_SIMPLE:
					anim_id = activeAnimator->CreateSimpleAnimation(animation_name);
					break;
				case AnimationType::AT_PARTIAL_BLEND:
					anim_id = activeAnimator->CreatePartialAnimation(animation_name);
					break;
				default:
					break;
			}

			if (anim_id == INVALID_INDEX) {
				error_msg = "Animation name already exists.";
			}
			else {
				error_msg = "";
				animation_name = "";

				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			error_msg = "";
			animation_name = "";

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	// Show animation list
	if (ImGui::CollapsingHeader("Animation list"))
	{
		size_t anim_count = activeAnimator->getAnimationCount();

		for (size_t i = 0; i < anim_count; i++)
		{
			Model::AnimationData& a_data = activeAnimator->getAnimationAt(i);

			if (!a_data.animation) continue;

			ImGui::PushID(i);

			if (ImGui::TreeNode(a_data.name.c_str()))
			{
				OzzAnimation* animation = a_data.animation;
				AnimationType a_type = animation->getAnimationType();
				ImGui::Text("Status: ");
				ImGui::SameLine();

				OzzAnimation::Status a_status = animation->getStatus();

				a_status == OzzAnimation::Status::VALID ?
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Valid") :
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid");

				float playback_speed = animation->getPlaybackSpeed();

				if (ImGui::InputFloat("Playback speed", &playback_speed)) {
					animation->setPlaybackSpeed(playback_speed);
				}

				bool loop = animation->getLoop();

				if (ImGui::Checkbox("Loop", &loop)) {
					animation->setLoop(loop);
				}

				switch (a_type)
				{
				case AnimationType::AT_PARTIAL_BLEND:
					DrawPartialBlendingAnimation((OzzAnimationPartialBlending*)animation);
					break;
				case AnimationType::AT_SIMPLE:
					DrawSimpleAnimation((OzzAnimationSimple*)animation);
					break;
				default:
					break;
				}

				float time_ratio = animation->GetTimeRatio();

				if (ImGui::SliderFloat("Time", &time_ratio, 0.0f, 1.0f)) {
					animation->SetTimeRatio(time_ratio);
				}

				if (ImGui::Button("Play")) {
					activeAnimator->PlayAnimation(i);
					isPlaying = true;
				}

				ImGui::SameLine();

				if (ImGui::Button("Stop")) {
					activeAnimator->StopAnimation(false);
					isPlaying = false;
				}

				if (ImGui::Button("Delete")) {
					activeAnimator->RemoveAnimationAt(i);
					isPlaying = false;
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}
}

void PanelAnimation::DrawPartialBlendingAnimation(OzzAnimationPartialBlending* partial_animation)
{
	// Upper body root
	static int ubr = 0;
	static int prevubr = 0;

	ubr = partial_animation->GetUpperBodyRoot();
	prevubr = ubr;

	ozz::span<const char* const> bone_names = activeAnimator->getSkeletonBoneNames();

	if (ImGui::BeginCombo("Upper body root", bone_names[ubr]))
	{
		for (int n = 0; n < bone_names.size(); n++)
		{
			bool is_selected = (ubr == n); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(bone_names[n], is_selected))
			{
				ubr = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}

		ImGui::EndCombo();
	}

	if (ubr != prevubr) {
		partial_animation->SetUpperBodyRoot(ubr);
	}

	// Lower body animation
	const char* lower = "No lower animation";

	if (partial_animation->IsLowerAnimationLoaded()) {
		lower = partial_animation->GetLowerBodyFile();
	}

	ImGui::Text("Lower body animation");
	ImGui::PushID("lower_body");
	if (ImGui::Button(lower))
	{
		std::string filePath = FileDialog::OpenFile("Open Lower Animation (*.anim)\0*.anim\0");
		if (!filePath.empty() && filePath.ends_with(".anim"))
		{
			partial_animation->LoadLowerAnimation(filePath.c_str());
		}
	}
	ImGui::PopID();

	// Upper body animation
	const char* upper = "No upper animation";

	if (partial_animation->IsUpperAnimationLoaded()) {
		upper = partial_animation->GetUpperBodyFile();
	}

	ImGui::Text("Upper body animation");
	ImGui::PushID("upper_body");

	if (ImGui::Button(upper))
	{
		std::string filePath = FileDialog::OpenFile("Open Upper Animation (*.anim)\0*.anim\0");
		if (!filePath.empty() && filePath.ends_with(".anim"))
		{
			partial_animation->LoadUpperAnimation(fs::relative(filePath).string().c_str());
		}
	}
	ImGui::PopID();
}

void PanelAnimation::DrawSimpleAnimation(OzzAnimationSimple* simple_animation)
{
	const char* anim_file = "No animation";

	if (simple_animation->IsAnimationLoaded()) {
		anim_file = simple_animation->getAnimationPath();
	}

	ImGui::Text("Animation file");
	ImGui::PushID("animation_file");
	//app->gui->AssetContainer(anim_file);
	if (ImGui::Button(anim_file))
	{
		std::string filePath = FileDialog::OpenFile("Open Animation (*.anim)\0*.anim\0");
		if (!filePath.empty() && filePath.ends_with(".anim"))
		{
			simple_animation->LoadAnimation(fs::relative(filePath).string().c_str());
		}
	}
	ImGui::PopID();
}
