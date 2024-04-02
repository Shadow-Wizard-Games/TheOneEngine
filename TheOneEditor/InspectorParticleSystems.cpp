#include "InspectorParticleSystems.h"

#include "imgui.h"

void UIEmmiterWriteNode(Emmiter* emmiter)
{
	ImGui::InputInt("maxParticles", &emmiter->maxParticles);

	if (ImGui::Button("Reset Pool")) emmiter->RestartParticlePool();

	ImGui::InputFloat("Duration", &emmiter->duration);
	ImGui::InputFloat("Delay", &emmiter->delay);
	ImGui::Checkbox("Loop", &emmiter->isLooping);

	ImGui::Separator();

	// spawn add ----
	ImGui::SeparatorText("Spawn Module");

	ImGui::SameLine();

	ImGui::PushID("add_spawn_emmiter_window");
	if (ImGui::BeginMenu("+"))
	{
		if (ImGui::MenuItem("Constant"))
			emmiter->AddModule(SpawnEmmiterModule::CONSTANT);

		if (ImGui::MenuItem("Single Burst"))
			emmiter->AddModule(SpawnEmmiterModule::SINGLE_BURST);

		if (ImGui::MenuItem("Constant Burst"))
			emmiter->AddModule(SpawnEmmiterModule::CONSTANT_BURST);

		ImGui::EndMenu();
	}
	ImGui::PopID();

	// spawn draw ---
	if (emmiter->spawnModule) {
		switch (emmiter->spawnModule->type) {
		case SpawnEmmiterModule::CONSTANT:
			UIInspectorEmmiterSpawnModule((ConstantSpawnRate*)emmiter->spawnModule.get());
			break;

		case SpawnEmmiterModule::SINGLE_BURST:
			UIInspectorEmmiterSpawnModule((SingleBurstSpawn*)emmiter->spawnModule.get());
			break;

		case SpawnEmmiterModule::CONSTANT_BURST:
			UIInspectorEmmiterSpawnModule((ConstantBurstSpawn*)emmiter->spawnModule.get());
			break;

		default:
			break;
		}
		ImGui::Separator();
	}
	
	// init add ----
	ImGui::SeparatorText("Initialize Modules");

	ImGui::SameLine();

	ImGui::PushID("add_init_emmiter_window");
	if (ImGui::BeginMenu("+"))
	{
		if (ImGui::MenuItem("Set Speed"))
			emmiter->AddModule(InitializeEmmiterModule::SET_SPEED);

		if (ImGui::MenuItem("Set Color"))
			emmiter->AddModule(InitializeEmmiterModule::SET_COLOR);

		if (ImGui::MenuItem("Set Scale"))
			emmiter->AddModule(InitializeEmmiterModule::SET_SCALE);

		ImGui::EndMenu();
	}
	ImGui::PopID();

	// init draw ---
	int imGuiIDInit = 0;
	
	for (auto m = emmiter->initializeModules.begin(); m != emmiter->initializeModules.end(); ++m) {
		ImGui::PushID("init_emmiter" + imGuiIDInit);

		if (ImGui::Button("Delete Module"))
		{
			auto moduleToDelete = m;
			++m;
			emmiter->initializeModules.erase(moduleToDelete);
			ImGui::PopID();
			break;
		}

		switch ((*m)->type) {
		case InitializeEmmiterModule::SET_SPEED:
			UIInspectorEmmiterInitializeModule((SetSpeed*)(*m).get());
			break;

		case InitializeEmmiterModule::SET_COLOR:
			UIInspectorEmmiterInitializeModule((SetColor*)(*m).get());
			break;

		case InitializeEmmiterModule::SET_SCALE:
			UIInspectorEmmiterInitializeModule((SetScale*)(*m).get());
			break;

		default:
			break;
		}
		ImGui::PopID();
		imGuiIDInit++;
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
	}

	ImGui::Separator();

	// update add ----
	ImGui::SeparatorText("Update Modules");

	ImGui::SameLine();

	ImGui::PushID("add_update_emmiter_window");
	if (ImGui::BeginMenu("+"))
	{
		if (ImGui::MenuItem("Acceleration"))
			emmiter->AddModule(UpdateEmmiterModule::ACCELERATION);

		ImGui::EndMenu();
	}
	ImGui::PopID();

	// update draw ---
	int imGuiIDUpdate = 0;

	for (auto m = emmiter->updateModules.begin(); m != emmiter->updateModules.end(); ++m) {
		ImGui::PushID("update_emmiter" + imGuiIDInit);
		
		if (ImGui::Button("Delete Module"))
		{
			auto moduleToDelete = m;
			++m;
			emmiter->updateModules.erase(moduleToDelete);
			ImGui::PopID();
			break;
		}

		switch ((*m)->type) {
		case UpdateEmmiterModule::ACCELERATION:
			UIInspectorEmmiterUpdateModule((AccelerationUpdate*)(*m).get());
			break;

		default:
			break;
		}
		ImGui::PopID();
		imGuiIDUpdate++;
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
	}

	ImGui::Separator();

	// render add ----
	ImGui::SeparatorText("Render Modules");

	ImGui::SameLine();

	ImGui::PushID("add_render_emmiter_window");
	if (ImGui::BeginMenu("+"))
	{
		if (ImGui::MenuItem("Billboard"))
			emmiter->AddModule(RenderEmmiterModule::BILLBOARD);

		ImGui::EndMenu();
	}
	ImGui::PopID();

	// render draw ---
	if (emmiter->renderModule) {
		switch (emmiter->spawnModule->type) {
		case RenderEmmiterModule::BILLBOARD:
			UIInspectorEmmiterRenderModule((BillboardRender*)emmiter->renderModule.get());
			break;
		default:
			break;
		}
	}

	ImGui::Separator();

}

// spawn modules ---------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterSpawnModule(ConstantSpawnRate* spawnModule) {
	ImGui::Text("Constant Spawn Rate");
	ImGui::InputFloat("Spawn Rate", &spawnModule->spawnRate);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}

void UIInspectorEmmiterSpawnModule(SingleBurstSpawn* spawnModule) {
	ImGui::Text("Single Burst");
	ImGui::InputFloat("Amount", &spawnModule->amount);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}

void UIInspectorEmmiterSpawnModule(ConstantBurstSpawn* spawnModule) {
	ImGui::Text("Constant Burst");
	ImGui::InputFloat("Amount", &spawnModule->amount);
	ImGui::InputFloat("Spawn Rate", &spawnModule->spawnRate);
	ImGui::InputFloat("Particle Duration", &spawnModule->duration);
}


// initialize modules ----------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterInitializeModule(SetSpeed* initModule)
{
	ImGui::Text("Set Initial Speed: ");

	ImGui::Checkbox("Single Value", &initModule->speed.usingSingleValue);

	ImGui::PushItemWidth(60);

	if (initModule->speed.usingSingleValue) {
		ImGui::PushID("set_speed_single_PS");
		ImGui::InputDouble("X", &initModule->speed.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_speed_min_PS");
		ImGui::InputDouble("X", &initModule->speed.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_speed_max_PS");
		ImGui::InputDouble("X", &initModule->speed.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->speed.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->speed.rangeValue.upperLimit.z, 0, 0, "%.2f");
		ImGui::PopID();
	}

	ImGui::PopItemWidth();

}

void UIInspectorEmmiterInitializeModule(SetColor* initModule)
{
	ImGui::Text("Set Initial Color: ");

	ImGui::Checkbox("Single Value", &initModule->color.usingSingleValue);

	ImGui::PushItemWidth(60);

	if (initModule->color.usingSingleValue) {
		ImGui::PushID("set_color_single_PS");
		ImGui::InputDouble("R", &initModule->color.singleValue.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.singleValue.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.singleValue.b, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_color_min_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.lowerLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.lowerLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.lowerLimit.b, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_color_max_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.upperLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.upperLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.upperLimit.b, 0, 0, "%.2f");
		ImGui::PopID();
	}

	ImGui::PopItemWidth();

}

void UIInspectorEmmiterInitializeModule(SetScale* initModule)
{
	ImGui::Text("Set Initial Scale: ");

	ImGui::Checkbox("Single Value", &initModule->scale.usingSingleValue);

	ImGui::PushItemWidth(60);

	if (initModule->scale.usingSingleValue) {
		ImGui::PushID("set_scale_single_PS");
		ImGui::InputDouble("R", &initModule->scale.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->scale.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->scale.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_scale_min_PS");
		ImGui::InputDouble("R", &initModule->scale.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->scale.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->scale.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_scale_max_PS");
		ImGui::InputDouble("R", &initModule->scale.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->scale.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->scale.rangeValue.upperLimit.z, 0, 0, "%.2f");
		ImGui::PopID();
	}

	ImGui::PopItemWidth();
}


// update modules --------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterUpdateModule(AccelerationUpdate* updateModule)
{
	ImGui::Text("Acceleration: ");

	ImGui::PushItemWidth(60);

	ImGui::PushID("set_color_max_PS");
	ImGui::InputDouble("R", &updateModule->acceleration.x, 0, 0, "%.2f");
	ImGui::SameLine();
	ImGui::InputDouble("G", &updateModule->acceleration.y, 0, 0, "%.2f");
	ImGui::SameLine();
	ImGui::InputDouble("B", &updateModule->acceleration.z, 0, 0, "%.2f");
	ImGui::PopID();

	ImGui::PopItemWidth();

}





// render modules --------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule)
{
	ImGui::Text("Billboard Render");
}
