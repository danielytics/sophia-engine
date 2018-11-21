
#include <concurrentqueue.h>
#include <lua.hpp>
#include <physfs.hpp>
#include <entt/entt.hpp>

#include "window/Window.h"
#include "graphics/DeferredRenderer.h"
#include "util/Telemetry.h"
#include "util/Config.h"
#include "util/Logging.h"
#include "util/Helpers.h"

#include "ecs/Loader.h"

#include "physics/Engine.h"

#ifdef USE_EASTL
// Declare new operators as needed by EASTL
#include <new>
#include <xmmintrin.h> // needed for _mm_malloc
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return ::operator new(size);
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
    return _mm_malloc(size, alignment); // Handle alignmentOffset?
}
#endif

void setupPhysFS (const char* argv0, const YAML::Node& config)
{
    PhysFS::init(argv0);
    // Mount game sources to search path
    {
        std::vector<std::string> paths;
        auto parser = Config::make_parser(
                    Config::map("game",
                        Config::sequence("sources", paths)
        ));
        parser(config);
        for (auto path : paths) {
            PhysFS::mount(path, "/", 1);
        }
    }
}

YAML::Node loadGameConfig (const YAML::Node& config)
{
    std::string configFile;
    auto parser = Config::make_parser(
                Config::map("game",
                    Config::scalar("game_config", configFile)));
    parser(config);
    return YAML::Load(Helpers::readToString(configFile));
}

void openWindow(Window& window, const YAML::Node& config, const YAML::Node& game_config)
{
    std::string gameName;
    auto parser = Config::make_parser(
                Config::map("game",
                    Config::scalar("name", gameName)));
    parser(game_config);
    window.open(gameName, config);
}

#include "ecs/systems/sprite_render.h"

void startSystems (graphics::Renderer& renderer) {
    ecs::System* sprite_render_system = new systems::sprite_render_system<>(renderer);
    ecs::System* sprite_shadow_render_system = new systems::sprite_render_system<>(renderer);
}

int main(int, char *argv[])
{
    YAML::Node config = YAML::LoadFile("config.yml");
    Logging::init(config);

    // Initialise and configure PhysicsFS
    setupPhysFS(argv[0], config);

    try {
        DeferredRenderer renderer;
        Window window(renderer);
        physics::Engine physicsEngine;
        entt::DefaultRegistry registry;
        ecs::loader::EntityLoader loader(registry);

        // Configure the game
        {
            YAML::Node game_config = loadGameConfig(config);
            openWindow(window, config, game_config);
            physicsEngine.init(game_config); // TODO: move into system
            startSystems(renderer);
            loader.load(game_config);
        }

        // Destroy the YAML configuration data
        config.reset();

        // Run the game
        window.run();
    }
    catch (const std::runtime_error& except) {
        error("Terminating due to: {}", except.what());
    }

    PhysFS::deinit();
    Logging::term();
    return 0;
}
