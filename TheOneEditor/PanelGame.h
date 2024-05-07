#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__
#pragma once

#include "Panel.h"
#include "Gui.h"

#include "TheOneEngine/Defs.h"
#include "TheOneEngine/Framebuffer.h"

#include <vector>
#include <memory>

class GameObject;
class Camera;
class Scene;

class PanelGame : public Panel
{
public:
	PanelGame(PanelType type, std::string name);
	~PanelGame();

	void Start();
	bool Draw();

	void OnSceneChange();
	std::vector<GameObject*> GetGameCameras();

	Scene* currentScene;
	Camera* gameCamera;

	std::shared_ptr<FrameBuffer> frameBuffer;
	vec2f viewportSize;

private:

	bool isHovered;
	bool isFocused;

	Aspect aspect;
	const char* aspects[3] = { "Free Aspect", "16:9 Aspect", "21:9 Aspect" };
};

#endif // !__PANEL_SCENE_H__