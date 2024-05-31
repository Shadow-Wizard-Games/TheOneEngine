#include "Window.h"
#include "Log.h"
#include "EngineCore.h"


Window::Window() :
    window(nullptr),
    glContext(),
    displayMode(DisplayMode::WINDOWED),
    resolution(Resolution::R_1280x720),
    borderless(false),
    refreshRate(0),
    minWidth(640),
    minHeight(360)
{}

Window::~Window() {}

bool Window::Awake()
{
    bool ret = true;

    SetResolution(resolution);

    LOG(LogType::LOG_INFO, "# Initializing SDL Window with OpenGL...");
    if (!initSDLWindowWithOpenGL())
        return false;

    LOG(LogType::LOG_INFO, "# Initializing OpenGL...");
    if (!createSdlGlContext())
        return false;

    if (!initOpenGL())
        return false;

    return true;
}

bool Window::Start()
{
    displayMode = DisplayMode::WINDOWED;
    resolution = Resolution::R_1280x720;

    SetResolution(resolution);

    return true;
}

bool Window::CleanUp()
{
    if (glContext) SDL_GL_DeleteContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

bool Window::initSDLWindowWithOpenGL()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Init SDL video subsystem: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Init SDL video subsystem");


    SDL_version compiled;
    SDL_VERSION(&compiled);
    LOG(LogType::LOG_OK, "-SDL Compiled with %d.%d.%d", compiled.major, compiled.minor, compiled.patch);

    SDL_version linked;
    SDL_GetVersion(&linked);
    LOG(LogType::LOG_OK, "-SDL Linked with %d.%d.%d", linked.major, linked.minor, linked.patch);

    // setup SDL window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // GL 3.1 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    std::string title = std::string(TITLE) + "_" + VERSION;
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    SDL_SetWindowMinimumSize(window, minWidth, minHeight);

    if (!window)
    {
        LOG(LogType::LOG_ERROR, "-Creating SDL window: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Creating SDL Window");

    return true;
}

// hekbas SDL_Surface needed?
// hekbas MUST move glContext and OpenGL Init to ENGINE
bool Window::createSdlGlContext()
{
    glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        LOG(LogType::LOG_ERROR, "-Creating OpenGL context: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Creating OpenGL context");

    if (SDL_GL_MakeCurrent(window, glContext) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Setting current OpenGL context : % s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Setting current OpenGL context");

    if (SDL_GL_SetSwapInterval(1) != 0)
    {
        LOG(LogType::LOG_ERROR, "-Setting swap interval for vsync: %s", SDL_GetError());
        return false;
    }
    LOG(LogType::LOG_OK, "-Setting swap interval for vsync");

    return true;
}

bool Window::initOpenGL()
{
    glewExperimental = GL_TRUE;
    auto glew_init_error = glewInit();
    if (glew_init_error != GLEW_OK)
    {
        LOG(LogType::LOG_ERROR, "-Init Glew: %s", glewGetErrorString(glew_init_error));
        return false;
    }
    LOG(LogType::LOG_OK, "-Init Glew");

    if (!GLEW_VERSION_3_1)
    {
        LOG(LogType::LOG_ERROR, "-OpenGL 3.1 Not Supported!");
        return false;
    }
    LOG(LogType::LOG_OK, "-OpenGL 3.1 Supported");


    LOG(LogType::LOG_OK, "-Init OpenGL");

    return true;
}

uint Window::GetDisplayRefreshRate()
{
    SDL_DisplayMode desktopDisplay;

    if (SDL_GetDesktopDisplayMode(0, &desktopDisplay) == 0)
        refreshRate = desktopDisplay.refresh_rate;
    else
        LOG(LogType::LOG_ERROR, "Getting display refresh rate: %s", SDL_GetError());

    return refreshRate;
}

DisplayMode Window::GetDisplayMode()
{
    return displayMode;
}

void Window::SetDisplayMode(DisplayMode mode)
{
    switch (mode)
    {
    case DisplayMode::WINDOWED:
    {
        if (SDL_SetWindowFullscreen(window, 0) != 0)
            LOG(LogType::LOG_ERROR, "Unable to set Display Mode to Windowed", SDL_GetError());

        OnResizeWindow(width, height);

        displayMode = DisplayMode::WINDOWED;
        break;
    }

    case DisplayMode::FULLSCREEN:
    {
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
            LOG(LogType::LOG_ERROR, "Unable to set Display Mode to Fullscreen Desktop", SDL_GetError());

        displayMode = DisplayMode::FULLSCREEN;
        break;
    }

    case DisplayMode::FULLSCREEN_DESKTOP:
    {
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
            LOG(LogType::LOG_ERROR, "Unable to set Display Mode to Fullscreen", SDL_GetError());

        displayMode = DisplayMode::FULLSCREEN_DESKTOP;
        break;
    }

    case DisplayMode::BORDERLESS:
    {
        SDL_SetWindowBordered(window, (SDL_bool)!borderless);
        displayMode = DisplayMode::BORDERLESS;
        break;
    }
    }
}

void Window::GetSDLWindowSize(int* w, int* h)
{
    SDL_GetWindowSize(window, w, h);
}

Resolution Window::GetResolution()
{
    return resolution;
}

void Window::SetResolution(Resolution res)
{
    switch (res)
    {
    case Resolution::R_3840x2160: width = 3840; height = 2160; resolution = Resolution::R_3840x2160; break;
    case Resolution::R_2560x1440: width = 2560; height = 1440; resolution = Resolution::R_2560x1440; break;
    case Resolution::R_1920x1080: width = 1920; height = 1080; resolution = Resolution::R_1920x1080; break;
    case Resolution::R_1280x720: width = 1280; height = 720; resolution = Resolution::R_1280x720; break;
    case Resolution::R_854x480: width = 854; height = 480; resolution = Resolution::R_854x480; break;
    case Resolution::R_640x360: width = 640; height = 360; resolution = Resolution::R_640x360; break;
    case Resolution::R_426x240: width = 426; height = 240; resolution = Resolution::R_426x240; break;
    case Resolution::R_NATIVE: /* Get native resolution */ resolution = Resolution::R_NATIVE; break;
    }

    OnResizeWindow(width, height);
}

void Window::OnResizeWindow(int width, int height)
{
    SDL_SetWindowSize(window, width, height);
    //app->engine->OnWindowResize(0, 0, width, height);
}

void Window::InfiniteScroll(bool global)
{
    if (global)
    {
        // hekbas: Implemet this if I have time :3
        //SDL_WarpMouseGlobal();
    }
    else
    {
        int width, height;
        GetSDLWindowSize(&width, &height);

        int mouseX = engine->inputManager->GetMouseX();
        int mouseY = engine->inputManager->GetMouseY();

        int warpToX = -1;
        int warpToY = -1;

        if (mouseX < 5) warpToX = width - 10;
        else if (mouseX > width - 5) warpToX = 10;

        if (mouseY < 5) warpToY = height - 10;
        else if (mouseY > height - 5) warpToY = 10;

        if (warpToX == -1 && warpToY == -1) return;

        if (warpToX == -1) warpToX = mouseX;
        if (warpToY == -1) warpToY = mouseY;

        SDL_WarpMouseInWindow(window, warpToX, warpToY);
    }
}