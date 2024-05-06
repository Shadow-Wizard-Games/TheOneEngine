#ifndef __WINDOW_H__
#define __WINDOW_H__
#pragma once

#include "Module.h"
#include "../TheOneEngine/Defs.h"

#include <SDL2/SDL.h>


//#define WINDOW_WIDTH 1280
//#define WINDOW_HEIGHT 720


enum class DisplayMode
{
    WINDOWED,
    FULLSCREEN,
    FULLSCREEN_DESKTOP,
    BORDERLESS,
};

enum class Resolution
{
    R_3840x2160,
    R_2560x1440,
    R_1920x1080,
    R_1280x720,
    R_854x480,
    R_640x360,
    R_426x240,
    R_NATIVE
};

class Window : public Module
{
public:
    Window(App* app);
    virtual ~Window();

    bool Awake();
    bool Start();
    bool CleanUp();

    bool initSDLWindowWithOpenGL();
    bool createSdlGlContext();
    bool initOpenGL();

    uint GetDisplayRefreshRate();

    DisplayMode GetDisplayMode();
    void SetDisplayMode(DisplayMode mode);

    void GetSDLWindowSize(int* w, int* h);

    Resolution GetResolution();
    void SetResolution(Resolution res);

    void OnResizeWindow(int width, int height);

    void InfiniteScroll(bool global = false);

    uint GetWidth() { return width; }
    uint GetHeight() { return height; }

public:
    SDL_Window* window;
    SDL_GLContext glContext;

private:
    uint refreshRate;

    DisplayMode displayMode;
    bool borderless;

    Resolution resolution;
    uint width;
    uint height;
    uint minWidth;
    uint minHeight;
};

#endif // !__WINDOW_H__