
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>
#include <physfs.h>

#include <iostream>

#include "window/Window.h"
#include "util/Telemetry.h"
#include "util/Config.h"
#include "util/Logging.h"

#include "physics/Engine.h"

int main(int argc, char *argv[])
{
    YAML::Node config = YAML::LoadFile("config.yml");
    Logging::init(config);

    // Initialise and configure PhysicsFS
    PHYSFS_init(argv[0]);
    {
        std::vector<std::string> paths;
        auto parser = Config::make_parser(
                    Config::map("game",
                        Config::sequence("sources", paths)
        ));
        parser(config);
        for (auto path : paths) {
            if (PHYSFS_mount(path.c_str(), "/", 1) == 0) {
                warn("Could not add {} to search path", path);
            }
        }
    }

    try {
        Window window("Sophia");
        window.open(config);
        physics::Engine physicsEngine;
        {
            std::string configFile;
            auto parser = Config::make_parser(Config::scalar("game_config", configFile));
            parser(config);
            PHYSFS_file* gameConfigFile = PHYSFS_openRead(configFile.c_str());
//            YAML::Node game_config = YAML::Load();
            physicsEngine.init(config);
        }

        // Destroy the YAML configuration data
        config.reset();

        window.run();
    }
    catch (const std::runtime_error& except) {
        error("Terminating due to: {}", except.what());
    }

    PHYSFS_deinit();
    Logging::term();
    return 0;
}

