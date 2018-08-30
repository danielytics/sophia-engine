#ifndef WINDOW_H
#define WINDOW_H

#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include "util/Config.h"

#include <string>

class Window
{
public:
    Window (class DeferredRenderer& renderer);
    ~Window();

    void open (const std::string& title, const YAML::Node&);
    void run ();

    GLuint u_current_time;

private:
    bool ready;
    SDL_GLContext context;
    SDL_Window* window;
    glm::vec4 viewport;
    float width;
    float height;
    class DeferredRenderer& renderer;
#ifdef DEBUG_BUILD
    bool debugMode;
#endif
};

#endif // WINDOW_H
