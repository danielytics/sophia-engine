#ifndef WINDOW_H
#define WINDOW_H

#ifndef GL3_PROTOTYPES
#define GL3_PROTOTYPES 1
#endif
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>

#include "util/config.h"

#include <string>

class Window
{
public:
    Window (const std::string& tile);
    ~Window();

    void open (const YAML::Node&);
    void run ();



    GLuint u_current_time;

private:
    bool ready;
    SDL_GLContext context;
    SDL_Window* window;
    glm::mat4 projection;
    glm::vec4 viewport;
    const std::string title;
    float width;
    float height;
};

#endif // WINDOW_H
