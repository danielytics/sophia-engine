
#include "window/window.h"
#include "util/logging.h"
#include "util/telemetry.h"
#include "util/config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "util/stb_image.h"

#include "graphics/shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <stdexcept>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
typedef double Time_t;
typedef std::chrono::duration<Time_t> Time;

struct MeshData {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> normals;
    std::vector<GLushort> indices;
    std::vector<glm::vec2> texcoords;
    enum { COMPONENTS_PER_VERTEX = 2,
           COMPONENTS_PER_COLOR = 4,
           COMPONENTS_PER_NORMAL = 3,
           COMPONENTS_PER_INDEX = 1,
           COMPONENTS_PER_UV = 2,
    };
};

class Mesh {
public:
    Mesh ();

    void init (const MeshData& data);
    void draw ();

private:
    // Create variables for storing the ID of our VAO and VBO
    enum {VBO=0, CBO, UV, NBO, IBO, VBO_COUNT};
    GLuint vao, vbo[VBO_COUNT];

    int vertices;
    int indices;

};

Mesh::Mesh ()
{

}

template <typename T>
void loadBuffer (GLenum bufferType, GLuint componentsPerVertex, GLuint attribId, GLuint buffer, const std::vector<T>& data)
{
    glBindBuffer(bufferType, buffer);
    // Copy the vertex data from diamond to our buffer
    auto vertexData = reinterpret_cast<const float*>(data.data());
    glBufferData(GL_ARRAY_BUFFER, data.size() * componentsPerVertex * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
    // Specify that our coordinate data is going into attribute index 0, and contains three floats per vertex
    glVertexAttribPointer(attribId, componentsPerVertex, GL_FLOAT, GL_FALSE, 0, 0);
}

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

void Mesh::init (const MeshData& data)
{
    vertices = data.vertices.size();
    indices = data.indices.size();

    // Generate and assign two Vertex Buffer Objects to our handle
    glGenBuffers(VBO_COUNT, vbo);

    // Generate and assign a Vertex Array Object to our handle
    glGenVertexArrays(1, &vao);

    // Bind our Vertex Array Object as the current used object
    glBindVertexArray(vao);

    // Positions
    // ===================
    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_VERTEX, VBO, vbo[VBO], data.vertices);
    glEnableVertexAttribArray(VBO);

    // Colors
    // =======================
    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_COLOR, CBO, vbo[CBO], data.colors);
    glEnableVertexAttribArray(CBO);

    // Texture coordinates
    // =======================
    loadBuffer(GL_ARRAY_BUFFER, MeshData::COMPONENTS_PER_UV, UV, vbo[UV], data.texcoords);
    glEnableVertexAttribArray(UV);


    // Indices
    // =======================
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[IBO]);

    // Copy the vertex data from diamond to our buffer
    auto indexData = reinterpret_cast<const GLshort*>(data.indices.data());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * MeshData::COMPONENTS_PER_INDEX * sizeof(GLshort), indexData, GL_STATIC_DRAW);
}

void Mesh::draw ()
{
//    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, vertices, 4);
    glDrawElementsInstanced(GL_TRIANGLES, indices, GL_UNSIGNED_SHORT, 0, 4);
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
        "     int offset = (gl_InstanceID % 6) * 6;"
        "     float x  = texelFetch(u_tbo_tex, offset + 0).r;"
        "     float y  = texelFetch(u_tbo_tex, offset + 1).r;"
        "     float vx = texelFetch(u_tbo_tex, offset + 2).r;"
        "     float vy = texelFetch(u_tbo_tex, offset + 3).r;"
        "     float t  = texelFetch(u_tbo_tex, offset + 4).r;"
        "     image = texelFetch(u_tbo_tex, offset + 5).r;"
        "     x = x + (vx * (u_current_time - t));"
        "     y = y + (vy * (u_current_time - t));"
        "     gl_Position = u_projection * u_view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);"
        "     uv = in_UV;"
        "     color = in_Color;"
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

    auto vao = Mesh{};
    {
        MeshData mesh = MeshData{};
        mesh.vertices = std::vector<glm::vec2>{
            {-0.5,  0.5},
            { 0.5,  0.5},
            { 0.5, -0.5},
            {-0.5, -0.5},
        };
        mesh.colors = std::vector<glm::vec4>{
            {1.0, 1.0, 1.0, 0.0},
            {1.0, 1.0, 1.0, 0.0},
            {1.0, 1.0, 1.0, 0.0},
            {1.0, 1.0, 1.0, 0.0},
        };
        mesh.texcoords = std::vector<glm::vec2>{
           {0.0f, 1.0f},
           {1.0f, 1.0f},
           {1.0f, 0.0f},
           {0.0f, 0.0f},
        };
        mesh.indices = std::vector<GLushort>{
            0, 1, 2,
            2, 3, 0,
        };
        vao.init(mesh);
    }

    float start_time_f = std::chrono::duration_cast<Time>(Clock::now().time_since_epoch()).count();
    float sprite_positions[] = {
      // x, y,   vx, vy, last_update_time, image
      1.0f, 0.0f, 0, 1,  start_time_f, 0,
      0.0f, 0.0f, 0,-1,  start_time_f, 0,
      0.0f, 0.0f, 0, 0,  start_time_f, 1,
     -1.0f, 1.0f, 0, 0.1,  start_time_f, 1,
     -1.0f,-1.0f, 0,-0.1,  start_time_f, 0,
      1.0f,-1.0f,-1.1,0.00,  start_time_f, 1,
    };

    GLuint tbo;
    GLuint tbo_tex;

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(sprite_positions), sprite_positions, GL_STATIC_DRAW);
    glGenTextures(1, &tbo_tex);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    GLuint u_tbo_tex = glGetUniformLocation(shader.programID, "u_tbo_tex");
    GLuint u_current_time = glGetUniformLocation(shader.programID, "u_current_time");
    GLuint u_projection = glGetUniformLocation(shader.programID, "u_projection");
    GLuint u_view = glGetUniformLocation(shader.programID, "u_view");
    GLuint u_texture = glGetUniformLocation(shader.programID, "u_texture");

    GLuint texture = loadTextureArray(std::vector<std::string>{
        "data/sprite.png",
        "data/sprite2.png",
    });

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
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader::use(shader);

        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glUniform1i(u_texture, 1);

        glUniform1f(u_current_time, std::chrono::duration_cast<Time>(current_time.time_since_epoch()).count());
        glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec3 Eye = glm::vec3(0.0f, -0.0f, 0.5f);
        glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0);
        glm::mat4 view = glm::rotate(glm::lookAt(Eye, Center, Up), glm::radians((float)std::chrono::duration_cast<Time>(current_time - start_time).count() * 30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));
        vao.draw();

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
