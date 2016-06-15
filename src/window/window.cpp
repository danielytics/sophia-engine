
#include "window/window.h"
#include "util/logging.h"
#include "util/telemetry.h"
#include "util/config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

#include "graphics/shader.h"
#include "graphics/mesh.h"

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

//struct MeshData {
//    std::vector<glm::vec2> vertices;
//    std::vector<glm::vec4> colors;
//    std::vector<glm::vec3> normals;
//    std::vector<GLushort> indices;
//    std::vector<glm::vec2> texcoords;
//    enum { COMPONENTS_PER_VERTEX = 2,
//           COMPONENTS_PER_COLOR = 4,
//           COMPONENTS_PER_NORMAL = 3,
//           COMPONENTS_PER_INDEX = 1,
//           COMPONENTS_PER_UV = 2,
//    };
//};

//class Mesh {
//public:
//    Mesh ();

//    void init (const MeshData& data);
//    void draw ();

//private:
//    // Create variables for storing the ID of our VAO and VBO
//    enum {VBO=0, CBO, UV, NBO, IBO, VBO_COUNT};
//    GLuint vao, vbo[VBO_COUNT];

//    int vertices;
//    int indices;

//};

//Mesh::Mesh ()
//{

//}


//class VAO {
//public:
//    VAO () {
//        glGenVertexArrays(1, &vao);
//        glBindVertexArray(vao);
//    }

//    template <typename T>
//    unsigned addBuffer (const std::vector<T>& data, bool vertices=false) {
//        GLuint id = vbos.size();
//        GLuint vbo;
//        glGenBuffers(1, &vbo);
//        loadBuffer(GL_ARRAY_BUFFER, VBOComponents<T>::NumComponents, id, vbo, data);
//        glEnableVertexAttribArray(id);
//        if (vertices) {
//            count = data.size();
//        }
//        vbos.push_back(vbo);
//        return id;
//    }

//    void set (unsigned id, bool enabled) {
//        if (enabled) {
//            glEnableVertexAttribArray(id);
//        } else {
//            glDisableVertexAttribArray(id);
//        }
//    }

//    void draw (unsigned int instances) {
////        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, count, instances);
//        glDrawArrays(GL_TRIANGLE_STRIP, 0, count);
//    }

//private:
//    GLuint count;
//    GLuint vao;
//    std::vector<GLuint> vbos;

//};


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

//void Mesh::init (const MeshData& data)
//{
//    vertices = data.vertices.size();
//    indices = data.indices.size();

//    // Generate and assign two Vertex Buffer Objects to our handle
//    glGenBuffers(VBO_COUNT, vbo);

//    // Generate and assign a Vertex Array Object to our handle
//    glGenVertexArrays(1, &vao);

//    // Bind our Vertex Array Object as the current used object
//    glBindVertexArray(vao);

//    // Positions
//    // ===================
//    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_VERTEX, VBO, vbo[VBO], data.vertices);
//    glEnableVertexAttribArray(VBO);

//    // Colors
//    // =======================
//    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_COLOR, CBO, vbo[CBO], data.colors);
//    glEnableVertexAttribArray(CBO);

//    // Texture coordinates
//    // =======================
//    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_UV, UV, vbo[UV], data.texcoords);
//    glEnableVertexAttribArray(UV);


//    // Indices
//    // =======================
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[IBO]);

//    // Copy the vertex data from diamond to our buffer
//    auto indexData = reinterpret_cast<const GLshort*>(data.indices.data());
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * MeshData::COMPONENTS_PER_INDEX * sizeof(GLshort), indexData, GL_STATIC_DRAW);
//}

//void Mesh::draw ()
//{
////    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertices, 4);
//    glDrawElementsInstanced(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, 0, 6);
//}



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

    projection = glm::perspective(60.0f, static_cast<float>(config.resolution.width) / static_cast<float>(config.resolution.height), 0.1f, 512.0f);

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
    std::string vertexShader =
        "#version 330 core"
        "   layout(location = 0) in vec2 in_Position;"
        "   layout(location = 1) in vec4 in_Color;"
        "   layout(location = 2) in vec2 in_UV;"
        "   uniform float u_current_time;"
        "   uniform samplerBuffer u_tbo_tex;"
        "	uniform mat4 u_projection;"
        "	uniform mat4 u_view;"
        "   out vec2 uv;"
        "   out float image;"
        "   out vec4 color;"
        "   void main() {"
        "     int offset = (gl_InstanceID % 6) * 3;"
        "     float x  = texelFetch(u_tbo_tex, offset + 0).r;"
        "     float y  = texelFetch(u_tbo_tex, offset + 1).r;"
        "     image = texelFetch(u_tbo_tex, offset + 5).r;"
        "     gl_Position = u_projection * u_view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);"
        "     color = in_Color;"
        "     uv = in_UV;"
        "   }";
    std::string fragmentShader =
        "   #version 330 core"
        "   in vec4 color;"
        "   out vec4 fragColor;"
        "   in vec2 uv;"
        "   in float image;"
        "   uniform sampler2DArray u_texture;"
        "   void main(void) {"
        "     fragColor = texture(u_texture, vec3(uv, image)).rgba * color;"
        "   }";
    Shader_t shader = Shader::load(vertexShader, fragmentShader);

    Shader_t tileMapShader = Shader::load(
        // VERTEX SHADER
        "#version 330 core"
        "   layout(location = 0) in vec2 in_Position;"
        "   layout(location = 1) in vec4 in_Color;"
        "   layout(location = 2) in vec2 in_UV;"
        "   layout(location = 3) in float in_Image;"
        "   uniform float u_current_time;"
        "	uniform mat4 u_projection;"
        "	uniform mat4 u_view;"
        "   out vec2 uv;"
        "   out vec4 color;"
        "   out float image;"
        "   void main() {"
        "     gl_Position = u_projection * u_view * vec4(in_Position, 0.0, 1.0);"
        "     color = in_Color;"
        "     uv = in_UV;"
        "     image = in_Image;"
        "   }",

        // FRAGMENT SHADER
        "   #version 330 core"
        "   in vec4 color;"
        "   out vec4 fragColor;"
        "   in vec2 uv;"
        "   in float image;"
        "   uniform sampler2DArray u_texture;"
        "   void main(void) {"
        "     fragColor = texture(u_texture, vec3(uv, image)).rgba * color;"
        "   }");

    auto mesh = Mesh{};
    {
        auto vertices = std::vector<glm::vec2>{};
        auto colours =  std::vector<glm::vec4>{};
        auto texcoords = std::vector<glm::vec2>{};
        auto images = std::vector<float>{};
        // Generate grid
        for (float y = 0; y < 100.0f; y += 1.0f) {
            for (float x = 0; x < 100.0f; x += 1.0f) {
                vertices.push_back(glm::vec2{x, y});
                colours.push_back(glm::vec4{1.0f, 1.0f, 1.0f, 0.0f});
                texcoords.push_back(glm::vec2{1.0f - std::fmod(x, 2.0f), std::fmod(y, 2.0f)});
                if (std::fmod(x, 10.0f) == 0 || std::fmod(y, 15.0f) == 0) {
                    images.push_back(1);
                } else {
                    images.push_back(0);
                }
            }
        }
        // Upload grid data to VBO
        mesh.addBuffer(vertices, true);
        mesh.addBuffer(colours);
        mesh.addBuffer(texcoords);
        mesh.addBuffer(images);
        mesh.addIndexBuffer();
    }

    struct SpritePosition {
        float x, y;
        float image;
    };
    auto sprites = std::vector<SpritePosition>{
        { 1.0f, 0.0f, 0},
        { 0.0f, 0.0f, 0},
        { 0.5f, 0.5f, 1},
        {-1.0f, 1.0f, 1},
        {-1.0f,-1.0f, 0},
        { 1.0f,-1.0f, 1},
    };


    std::vector<glm::vec3> spriteData;
    for (auto sprite : sprites) {
        spriteData.push_back(glm::vec3{sprite.x, sprite.y, sprite.image});
    }


    GLuint tbo;
    GLuint tbo_tex;

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteData.size(), reinterpret_cast<float*>(spriteData.data()), GL_STREAM_DRAW);
    glGenTextures(1, &tbo_tex);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    GLuint u_tbo_tex = glGetUniformLocation(shader.programID, "u_tbo_tex");
    GLuint u_current_time = glGetUniformLocation(tileMapShader.programID, "u_current_time");
    GLuint u_projection = glGetUniformLocation(tileMapShader.programID, "u_projection");
    GLuint u_view = glGetUniformLocation(tileMapShader.programID, "u_view");
    GLuint u_texture = glGetUniformLocation(tileMapShader.programID, "u_texture");

    GLuint texture = loadTextureArray(std::vector<std::string>{
        "data/sprite.png",
        "data/sprite2.png",
    });

    glm::vec3 camera = glm::vec3(0.0f, 0.0f, 0.5f);
    glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

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
        float time_since_start = std::chrono::duration_cast<Time>(current_time - start_time).count();
        glm::mat4 view = glm::rotate(glm::lookAt(camera, glm::vec3{camera.x, camera.y, 0.0f}, Up), glm::radians(time_since_start * 30.0f) * 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        // Render frame
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

//        Shader::use(shader);

//        glUniform1i(u_texture, 1);

//        glUniform1f(u_current_time, time_since_start);
//        glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
//        glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));

        Shader::use(tileMapShader);
        glUniform1i(u_texture, 1);
        glUniform1f(u_current_time, time_since_start);
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));
        auto indices = std::vector<GLushort>{};
        GLushort xbase = (GLushort)(camera.x - 10.0f);
        GLushort yidx = (GLushort)(camera.y - 10.0f);
        GLushort xidx = xbase;
        for (float x = camera.x - 10.0f; x < camera.x + 10.0f; x += 1) {
            for (float y = camera.y - 10.0f; y < camera.y + 10.0f; y += 1) {
                auto index = xidx + (yidx * 100);
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 101);

                indices.push_back(index + 101);
                indices.push_back(index);
                indices.push_back(index + 100);

                ++xidx;
            }
            ++yidx;
            xidx = xbase;
        }
        mesh.drawIndexed(indices);
        /////////////////////////////////////
        // Clear screen
        // Draw tilemap
        // select tile render shader
        // upload tile indices for current viewport
        // draw tilemap vbo
        /////////////////////////////////////

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
