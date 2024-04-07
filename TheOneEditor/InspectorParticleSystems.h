#pragma once

#include "..\TheOneEngine\ParticleSystem.h"

void UIEmmiterWriteNode(Emmiter* emmiter);

// spawn modules
void UIInspectorEmmiterSpawnModule(ConstantSpawnRate* spawnModule);

void UIInspectorEmmiterSpawnModule(SingleBurstSpawn* spawnModule);

void UIInspectorEmmiterSpawnModule(ConstantBurstSpawn* spawnModule);



// initialize modules
void UIInspectorEmmiterInitializeModule(SetSpeed* initModule);

void UIInspectorEmmiterInitializeModule(SetColor* initModule);

void UIInspectorEmmiterInitializeModule(SetScale* initModule);



// update modules
void UIInspectorEmmiterUpdateModule(AccelerationUpdate* updateModule);



// render modules
void UIInspectorEmmiterRenderModule(BillboardRender* renderModule);


