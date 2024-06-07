#pragma once

#include "..\TheOneEngine\ParticleSystem.h"

void UIEmmiterWriteNode(Emmiter* emmiter, int ID);

// spawn modules
void UIInspectorEmmiterSpawnModule(ConstantSpawnRate* spawnModule);

void UIInspectorEmmiterSpawnModule(SingleBurstSpawn* spawnModule);

void UIInspectorEmmiterSpawnModule(ConstantBurstSpawn* spawnModule);



// initialize modules
void UIInspectorEmmiterInitializeModule(SetSpeed* initModule);

void UIInspectorEmmiterInitializeModule(SetColor* initModule);

void UIInspectorEmmiterInitializeModule(SetScale* initModule);

void UIInspectorEmmiterInitializeModule(SetOffset* initModule);

void UIInspectorEmmiterInitializeModule(SetDirection* initModule);

// update modules
void UIInspectorEmmiterUpdateModule(AccelerationUpdate* updateModule);

void UIInspectorEmmiterUpdateModule(ColorOverLifeUpdate* updateModule);

void UIInspectorEmmiterUpdateModule(ScaleOverLifeUpdate* updateModule);


// render modules
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule);


