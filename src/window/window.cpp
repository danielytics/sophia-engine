
#include "window/window.h"
#include "util/logging.h"
#include "util/telemetry.h"
#include "util/config.h"

#include <stdexcept>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef double Time_t;
typedef std::chrono::duration<Time_t> Time;


Window::Window (const std::string& title)
    : ready(false)
    , window(0)
    , title(title)
{
    // Initialize SDL's Video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        error("Failed to init SDL");
        throw std::runtime_error("Failed to initialise SDL");
    }

    ready = true;
}

Window::~Window ()
{
    if (ready) {
        if (window) {
            SDL_GL_DeleteContext(context);
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }
}

void Window::open ()
{
    // Load ystem configuration
    struct {
        int width;
        int height;
        int fsaa;
        bool vsync;
        bool fullscreen;
    } config;
    {
         using namespace Config;
         auto parser = make_parser(
            map("graphics",
                scalar("fullscreen", config.fullscreen),
                scalar("vsync", config.vsync),
                fn("resolution",
                    [&config](const YAML::Node& node) {
                        if (node.IsScalar()) {
                            try {
                                auto res = node.as<std::string>();
                                if (res == "720p") {
                                    config.width = 1280;
                                    config.height = 720;
                                } else if (res == "1080p") {
                                    config.width = 1920;
                                    config.height = 1080;
                                } else {
                                    return InvalidValue;
                                }
                            } catch (...) {
                                return BadTypeConversion;
                            }
                        } else if (node.IsSequence()) {
                            try {
                                auto res = node.as<std::vector<unsigned>>();
                                if (res.size() == 2) {
                                    config.width = res[0];
                                    config.height = res[1];
                                } else {
                                    return InvalidValue;
                                }
                            } catch (...) {
                                return BadTypeConversion;
                            }
                        } else {
                            return BadParse;
                        }
                        return Success;
                    }
                ),
                fn("fsaa",
                    [&config](const YAML::Node& node) {
                        if (node.IsScalar()) {
                            try {
                                auto fsaa = node.as<std::string>();
                                auto values = std::map<std::string, int>{
                                    {"2x", 2},
                                    {"4x", 4},
                                    {"8x", 8},
                                    {"16x", 16},
                                    {"Off", 0}
                                };
                                auto it = values.find(fsaa);
                                if (it != values.end()) {
                                    config.fsaa = it->second;
                                    return Success;
                                }
                                return InvalidValue;
                            } catch (...) {
                                return BadTypeConversion;
                            }
                        } else {
                            return NotScalar;
                        }
                    }
                )
            )
        );
        parser(YAML::LoadFile("config.yml"));
    }

    // Set the OpenGL attributes for our context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, config.fsaa > 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, config.fsaa);

    // Create a centered window, using system configuration
    window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                config.width,
                config.height,
                SDL_WINDOW_OPENGL | config.fullscreen);
    // Create OpenGL rendering context
    context = SDL_GL_CreateContext(window);

    // Set vertical sync if configured
    SDL_GL_SetSwapInterval(config.vsync);

    // Load OpenGL 3+ functions
    glewExperimental = GL_TRUE;
    glewInit();

    // Enable FSAA if configured
    if (config.fsaa > 0) {
        glEnable(GL_MULTISAMPLE);
    }
}

void Window::run ()
{
    SDL_Event event;
    bool running = true;

    // Initialise timekeeping
    float frame_time;
    auto start_time = Clock::now();
    auto previous_time = start_time;
    auto current_time = start_time;
    Telemetry::Counter frames("frames");

    // Run the main processing loop
    do {
        // Gather and dispatch input
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                default:
                    break;
                }
            }
        }

        // Render frame
        glUniform1f(u_current_time, std::chrono::duration_cast<Time>(current_time.time_since_epoch()).count());
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);



        // Refresh display
        SDL_GL_SwapWindow(window);

        // Update timekeeping
        previous_time = current_time;
        current_time = Clock::now();
        frame_time = std::chrono::duration_cast<Time>(current_time - previous_time).count();
        frames.inc();
        trace("Frame {} ended after {:1.6f} seconds", frames.get(), frame_time);
    } while (running);

    info("Average framerate: {} fps", (frames.get() / std::chrono::duration_cast<Time>(current_time - start_time).count()));
}
