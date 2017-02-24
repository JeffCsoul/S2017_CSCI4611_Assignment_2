#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <iostream>
#include <string>

class Engine {
public:
    Engine();
    ~Engine();
    SDL_Window* createWindow(std::string title, int width, int height);
    void destroyWindow(SDL_Window*);
    bool shouldQuit();
    void handleInput();
    void waitForNextFrame(float secondsPerFrame);
    // input state
    bool isKeyDown(int scancode);
    bool isMouseButtonDown(int button);
    void mousePosition(int *x, int *y);
    int mouseX();
    int mouseY();
    // callback functions; child classes should override
    virtual void onKeyDown(SDL_KeyboardEvent&) {}
    virtual void onKeyUp(SDL_KeyboardEvent&) {}
    virtual void onMouseMotion(SDL_MouseMotionEvent&) {}
    virtual void onMouseButtonDown(SDL_MouseButtonEvent&) {}
    virtual void onMouseButtonUp(SDL_MouseButtonEvent&) {}
protected:
    bool userQuit;
    int lastFrameTime;
    void die_with_sdl_error(std::string message);
    void die_if_opengl_error();
};

// Definitions below

inline void Engine::die_with_sdl_error(std::string message) {
    std::cout << message << ": " << SDL_GetError() << std::endl;
    exit(EXIT_FAILURE);
}

inline void Engine::die_if_opengl_error() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << gluErrorString(error) << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline Engine::Engine() {
    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status < 0)
        die_with_sdl_error("Failed to initialize SDL");
    userQuit = false;
    lastFrameTime = 0;
}

inline Engine::~Engine() {
    SDL_Quit();
}

inline SDL_Window* Engine::createWindow(std::string title, int width, int height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_Window *window =
        SDL_CreateWindow(title.c_str(),
                         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL)
        die_with_sdl_error("Failed to create window");
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL)
        die_with_sdl_error("Failed to create OpenGL context");
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    die_if_opengl_error();
    return window;
}

inline void Engine::destroyWindow(SDL_Window *window) {
    SDL_DestroyWindow(window);
}

inline void Engine::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            userQuit = true;
            break;
        case SDL_KEYDOWN:
            onKeyDown(event.key);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key);
            break;
        case SDL_MOUSEMOTION:
            onMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonDown(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonUp(event.button);
            break;
        }
    }
}

inline bool Engine::shouldQuit() {
    return userQuit;
}

inline void Engine::waitForNextFrame(float secondsPerFrame) {
    int millisPerFrame = (int)(1000*secondsPerFrame);
    int now = SDL_GetTicks();
    int nextFrameTime = lastFrameTime + millisPerFrame;
    lastFrameTime = now;
    if (nextFrameTime > now)
        SDL_Delay(nextFrameTime - now);
}

inline bool Engine::isKeyDown(int scancode) {
    return SDL_GetKeyboardState(NULL)[scancode];
}

inline bool Engine::isMouseButtonDown(int button) {
    return SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(button);
}

inline void Engine::mousePosition(int *x, int *y) {
    SDL_GetMouseState(x,y);
}

inline int Engine::mouseX() {
    int x;
    SDL_GetMouseState(&x,NULL);
    return x;
};

inline int Engine::mouseY() {
    int y;
    SDL_GetMouseState(NULL,&y);
    return y;
}

#endif
