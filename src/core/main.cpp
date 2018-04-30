
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>
#include <physfs.hpp>

#include <iostream>

#include "window/Window.h"
#include "util/Telemetry.h"
#include "util/Config.h"
#include "util/Logging.h"
#include "util/Helpers.h"

#include "physics/Engine.h"

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

int main(int, char *argv[])
{
    YAML::Node config = YAML::LoadFile("config.yml");
    Logging::init(config);

    // Initialise and configure PhysicsFS
    setupPhysFS(argv[0], config);

    try {
        Window window;
        physics::Engine physicsEngine;

        // Configure the game
        {
            YAML::Node game_config = loadGameConfig(config);
            openWindow(window, config, game_config);
            physicsEngine.init(game_config);
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

