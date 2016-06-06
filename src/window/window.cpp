
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

void Window::open (const YAML::Node& config_node)
{
    // Load ystem configuration
    struct Resolution {
        int width;
        int height;
        inline void operator= (const std::vector<int>& vec) {
            width = vec[0];
            height = vec[1];
        }
    };
    struct {
        Resolution resolution;
        int fsaa;
        bool vsync;
        bool fullscreen;
    } config;
    {
        std::vector<int> res;
         using namespace Config;
         auto parser = make_parser(
            map("graphics",
                scalar("fullscreen", config.fullscreen),
                scalar("vsync", config.vsync),
                one_of(
                    option("resolution",
                        std::map<std::string, Resolution>{
                            {"720p", Resolution{1280, 720}},
                            {"1080p", Resolution{1920, 1080}},
                        }, config.resolution),
                    sequence("resolution", res)
                    ),
                option("fsaa",
                    std::map<std::string, int>{
                        {"2x", 2},
                        {"4x", 4},
                        {"8x", 8},
                        {"16x", 16},
                        {"32x", 32},
                        {"Off", 0},
                    }, config.fsaa)
            )
        );
        parser(config_node);
        if (res.size()) {
            config.resolution = res;
        }
    }
    info("Loaded graphics configuration: fsaa={} vsync={} fullscreen={} width={} height={}", config.fsaa, config.vsync, config.fullscreen, config.resolution.width, config.resolution.height);

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
                config.resolution.width,
                config.resolution.height,
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
    auto frames = Telemetry::Counter{"frames"};

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
        glClearColor(0.0, 0.0, 0.0, 1.0);
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
