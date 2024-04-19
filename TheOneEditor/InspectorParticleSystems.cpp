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

		if (ImGui::MenuItem("Set Offset"))
			emmiter->AddModule(InitializeEmmiterModule::SET_OFFSET);

		if (ImGui::MenuItem("Set Direction"))
			emmiter->AddModule(InitializeEmmiterModule::SET_DIRECTION);


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

		case InitializeEmmiterModule::SET_OFFSET:
			UIInspectorEmmiterInitializeModule((SetOffset*)(*m).get());
			break;

		case InitializeEmmiterModule::SET_DIRECTION:
			UIInspectorEmmiterInitializeModule((SetDirection*)(*m).get());
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
		ImGui::InputFloat("X", &initModule->speed.singleValue, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_speed_min_PS");
		ImGui::InputFloat("X", &initModule->speed.rangeValue.lowerLimit, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_speed_max_PS");
		ImGui::InputFloat("X", &initModule->speed.rangeValue.upperLimit, 0, 0, "%.2f");
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
		ImGui::SameLine();
		ImGui::InputDouble("A", &initModule->color.singleValue.a, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_color_min_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.lowerLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.lowerLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.lowerLimit.b, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("A", &initModule->color.rangeValue.lowerLimit.a, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_color_max_PS");
		ImGui::InputDouble("R", &initModule->color.rangeValue.upperLimit.r, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("G", &initModule->color.rangeValue.upperLimit.g, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("B", &initModule->color.rangeValue.upperLimit.b, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("A", &initModule->color.rangeValue.upperLimit.a, 0, 0, "%.2f");
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
		ImGui::InputDouble("X", &initModule->scale.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->scale.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->scale.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_scale_min_PS");
		ImGui::InputDouble("X", &initModule->scale.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->scale.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->scale.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_scale_max_PS");
		ImGui::InputDouble("X", &initModule->scale.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->scale.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->scale.rangeValue.upperLimit.z, 0, 0, "%.2f");
		ImGui::PopID();
	}

	ImGui::PopItemWidth();
}

void UIInspectorEmmiterInitializeModule(SetOffset* initModule)
{
	ImGui::Text("Set Initial Offset: ");

	ImGui::Checkbox("Single Value", &initModule->offset.usingSingleValue);

	ImGui::PushItemWidth(60);

	if (initModule->offset.usingSingleValue) {
		ImGui::PushID("set_offset_single_PS");
		ImGui::InputDouble("X", &initModule->offset.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->offset.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->offset.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_offset_min_PS");
		ImGui::InputDouble("X", &initModule->offset.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->offset.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->offset.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_offset_max_PS");
		ImGui::InputDouble("X", &initModule->offset.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->offset.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->offset.rangeValue.upperLimit.z, 0, 0, "%.2f");
		ImGui::PopID();
	}

	ImGui::PopItemWidth();
}

void UIInspectorEmmiterInitializeModule(SetDirection* initModule)
{
	ImGui::Text("Set Initial Direction: ");

	ImGui::Checkbox("Single Value", &initModule->direction.usingSingleValue);

	ImGui::PushItemWidth(60);

	if (initModule->direction.usingSingleValue) {
		ImGui::PushID("set_direction_single_PS");
		ImGui::InputDouble("X", &initModule->direction.singleValue.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->direction.singleValue.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->direction.singleValue.z, 0, 0, "%.2f");
		ImGui::PopID();
	}
	else {
		ImGui::PushID("set_direction_min_PS");
		ImGui::InputDouble("X", &initModule->direction.rangeValue.lowerLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->direction.rangeValue.lowerLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->direction.rangeValue.lowerLimit.z, 0, 0, "%.2f");
		ImGui::PopID();

		ImGui::PushID("set_direction_max_PS");
		ImGui::InputDouble("X", &initModule->direction.rangeValue.upperLimit.x, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Y", &initModule->direction.rangeValue.upperLimit.y, 0, 0, "%.2f");
		ImGui::SameLine();
		ImGui::InputDouble("Z", &initModule->direction.rangeValue.upperLimit.z, 0, 0, "%.2f");
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
	ImGui::InputDouble("X", &updateModule->acceleration.x, 0, 0, "%.2f");
	ImGui::SameLine();
	ImGui::InputDouble("Y", &updateModule->acceleration.y, 0, 0, "%.2f");
	ImGui::SameLine();
	ImGui::InputDouble("Z", &updateModule->acceleration.z, 0, 0, "%.2f");
	ImGui::PopID();

	ImGui::PopItemWidth();

}





// render modules --------------------------------------------------------------------------------------------------------------
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule)
{
	ImGui::Text("Billboard Render");
}
