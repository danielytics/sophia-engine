
#include "window/window.h"
#include "util/logging.h"
#include "util/telemetry.h"
#include "util/config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

#include "graphics/shader.h"
#include "graphics/tilemap.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include <stdexcept>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef double Time_t;
typedef std::chrono::duration<Time_t> Time;

GLuint loadTexture (const std::string& filename)
{
    GLuint texture = 0;
    int width, height, comp;
    unsigned char* image = stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);
    if (image) {
        info("Loading image '{}', width={} height={} components={}", filename, width, height, comp);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        stbi_image_free(image);
    } else {
        error("Could not load texture: {}", filename);
    }
    return texture;
}

GLuint loadTextureArray (const std::vector<std::string>& filenames)
{
    GLuint texture = 0;
    int width, height;
    int components;
    // Load the image files
    auto images = std::vector<unsigned char*>{};
    for (auto filename : filenames) {
        unsigned char* image = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);
        info("Loading image '{}', width={} height={} components={}", filename, width, height, components);
        images.push_back(image);
    }

    // Create the texture array
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,  width, height, images.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Load texture data into texture array
    for (unsigned index = 0; index < images.size(); ++index) {
        auto image = images[index];
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
    }
    info("Loaded {} images into texture array", images.size());

    //Always set reasonable texture parameters
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    return texture;
}


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
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
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

    float width = static_cast<float>(config.resolution.width);
    float height = static_cast<float>(config.resolution.height);
//    projection = glm::perspective(60.0f, width / height, 0.1f, 100.0f);
    float w = (width / height) * 5.0f;
    projection = glm::ortho(-w, w, -5.0f, 5.0f, 0.0f, 10.0f);
    viewport = glm::vec4(0, 0, width, height);
    glViewport(0, 0, config.resolution.width, config.resolution.height);

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

    using Shader_t = Shader::Shader;
//    std::string vertexShader =
//        "#version 330 core"
//        "   layout(location = 0) in vec2 in_Position;"
//        "   layout(location = 1) in vec4 in_Color;"
//        "   layout(location = 2) in vec2 in_UV;"
//        "   uniform float u_current_time;"
//        "   uniform samplerBuffer u_tbo_tex;"
//        "	uniform mat4 u_projection;"
//        "	uniform mat4 u_view;"
//        "   out vec2 uv;"
//        "   out float image;"
//        "   out vec4 color;"
//        "   void main() {"
//        "     int offset = (gl_InstanceID % 6) * 3;"
//        "     float x  = texelFetch(u_tbo_tex, offset + 0).r;"
//        "     float y  = texelFetch(u_tbo_tex, offset + 1).r;"
//        "     image = texelFetch(u_tbo_tex, offset + 5).r;"
//        "     gl_Position = u_projection * u_view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);"
//        "     color = in_Color;"
//        "     uv = in_UV;"
//        "   }";
//    std::string fragmentShader =
//        "   #version 330 core"
//        "   in vec4 color;"
//        "   out vec4 fragColor;"
//        "   in vec2 uv;"
//        "   in float image;"
//        "   uniform sampler2DArray u_texture;"
//        "   void main(void) {"
//        "     fragColor = texture(u_texture, vec3(uv, image)).rgba * color;"
//        "   }";
    std::string vertexShader =
        "#version 330 core"
        "   layout(location = 0) in vec3 in_Position;"
        "	uniform mat4 u_projection;"
        "	uniform mat4 u_view;"
        "   void main() {"
//        "     gl_Position = u_view * u_projection * vec4(in_Position, 1.0);"
        "     gl_Position = u_projection * u_view * vec4(in_Position, 1.0);"
        "   }";
    std::string fragmentShader =
        "   #version 330 core"
        "   out vec4 fragColor;"
        "   void main(void) {"
        "     fragColor = vec4(1.0, 1.0, 1.0, 1.0);"
        "   }";
    Shader_t shader = Shader::load(vertexShader, fragmentShader);
    Shader::use(shader);
    GLuint u_projection = glGetUniformLocation(shader.programID, "u_projection");
    GLuint u_view = glGetUniformLocation(shader.programID, "u_view");


//    struct SpritePosition {
//        float x, y;
//        float image;
//    };
//    auto sprites = std::vector<SpritePosition>{
//        { 1.0f, 0.0f, 0},
//        { 0.0f, 0.0f, 0},
//        { 0.5f, 0.5f, 1},
//        {-1.0f, 1.0f, 1},
//        {-1.0f,-1.0f, 0},
//        { 1.0f,-1.0f, 1},
//    };


//    std::vector<glm::vec3> spriteData;
//    for (auto sprite : sprites) {
//        spriteData.push_back(glm::vec3{sprite.x, sprite.y, sprite.image});
//    }


//    GLuint tbo;
//    GLuint tbo_tex;

//    glGenBuffers(1, &tbo);
//    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
//    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteData.size(), reinterpret_cast<float*>(spriteData.data()), GL_STREAM_DRAW);
//    glGenTextures(1, &tbo_tex);
//    glBindBuffer(GL_TEXTURE_BUFFER, 0);
//    GLuint u_tbo_tex = glGetUniformLocation(shader.programID, "u_tbo_tex");
//    GLuint u_current_time = glGetUniformLocation(shader.programID, "u_current_time");

    TileMap tileMap;
    tileMap.init(std::vector<std::vector<float>>{
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,  },
        { 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,  },
        { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,  },
        { 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1,  },
        { 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  },
     });

    glm::vec3 camera = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0 + 0);
    GLuint texture = loadTextureArray(std::vector<std::string>{
        "data/images/Brown Block.png",
        "data/images/Grass Block.png",

    });
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

//    glActiveTexture(GL_TEXTURE0 + 1);
//    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
//    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(0); // Disable writing to depth buffer

    Mesh mesh;
    mesh.addBuffer(std::vector<glm::vec3>{
                       {-0.5f, 0.5f, 0.0f},
                       { 0.5f, 0.5f, 0.0f},
                       { 0.5f,-0.5f, 0.0f},
                       { 0.5f,-0.5f, 0.0f},
                       {-0.5f,-0.5f, 0.0f},
                       {-0.5f, 0.5f, 0.0f},

                       {1.0f + -0.5f, 1.0f + 0.5f, 0.0f},
                       {1.0f +  0.5f, 1.0f + 0.5f, 0.0f},
                       {1.0f +  0.5f, 1.0f + -0.5f, 0.0f},
                       {1.0f +  0.5f, 1.0f + -0.5f, 0.0f},
                       {1.0f + -0.5f, 1.0f + -0.5f, 0.0f},
                       {1.0f + -0.5f, 1.0f + 0.5f, 0.0f},
                   }, true);

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
                case SDLK_UP:
                    camera.y += frame_time * 5.0f;
                    break;
                case SDLK_DOWN:
                    camera.y -= frame_time * 5.0f;
                    break;
                case SDLK_LEFT:
                    camera.x -= frame_time * 5.0f;
                    break;
                case SDLK_RIGHT:
                    camera.x += frame_time * 5.0f;
                    break;
                default:
                    break;
                }
            }
        }


//        Shader::use(shader);
//        glUniform1i(u_texture, 1);
//        glUniform1f(u_current_time, time_since_start);
//        glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
//        glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));

        /////////////////////////////////////
        // Calculate current time
        float time_since_start = std::chrono::duration_cast<Time>(current_time - start_time).count();
        // Generate the view matrix using the camera position
//        glm::mat4 view = glm::rotate(glm::lookAt(camera, glm::vec3{camera.x, camera.y, 0.0f}, Up), glm::radians(time_since_start * 30.0f) * 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 view = glm::lookAt(camera, glm::vec3{camera.x, camera.y, camera.z - 1.0f}, Up);

        int x, y;
        SDL_GetMouseState(&x, &y);
        glm::vec3 mouse = glm::unProject(glm::vec3(x, viewport.w - y, 0.0f), view, projection, viewport);

        info("Camera: {}, {}, {}", camera.x, camera.y, camera.z);
        info("Mouse:  {}, {}, {}", mouse.x, mouse.y, mouse.z);
        info();

        // Clear screen
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw tilemap
        tileMap.render(camera, projection, view);
        // Draw sprites
        /////////////////////////////////////

        Shader::use(shader);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));
        mesh.draw();

//        // Render each viewport in turn
//        for (auto viewport : viewports) {
//            // Setup the viewport position and dimensions
//            glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
//            // Setup stencil buffer here?
//            // ...
//            // Render the viewports scene
//            // ...
//            // Cast shadows
//            for (auto light : viewport.lights) {
//                // Render from each lights pov to cast dynamic shadows
//                // ...
//            }
//            // Combine and postprocess
//            // ...
//        }

        // Refresh display
        SDL_GL_SwapWindow(window);

        // Update timekeeping
        previous_time = current_time;
        current_time = Clock::now();
        frame_time = std::chrono::duration_cast<Time>(current_time - previous_time).count();
        frames.inc();
        trace("Frame {} ended after {:1.6f} seconds", frames.get(), frame_time);
    } while (running);

    glDeleteTextures(1, &texture);
    Shader::unload(shader);

    info("Average framerate: {} fps", (frames.get() / std::chrono::duration_cast<Time>(current_time - start_time).count()));
}
