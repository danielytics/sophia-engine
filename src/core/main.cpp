
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>
#include <physfs.hpp>
#include <entt/entt.hpp>
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

#include <iostream>

#include "window/Window.h"
#include "util/Telemetry.h"
#include "util/Config.h"
#include "util/Logging.h"
#include "util/Helpers.h"

#include "ecs/Loader.h"

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

#if 0
#include <iostream>
#include <algorithm>
static const size_t N = 10;

class SubStringFinder {
   const std::string str;
   size_t *max_array;
   size_t *pos_array;
 public:
   void operator() ( const tbb::blocked_range<size_t>& r ) const {
       for ( size_t i = r.begin(); i != r.end(); ++i ) {
           size_t max_size = 0, max_pos = 0;
           for (size_t j = 0; j < str.size(); ++j)
               if (j != i) {
                   size_t limit = str.size()-std::max(i,j);
                   for (size_t k = 0; k < limit; ++k) {
                       if (str[i + k] != str[j + k]) break;
                       if (k > max_size) {
                           max_size = k;
                           max_pos = j;
                       }
                   }
               }
           max_array[i] = max_size;
           pos_array[i] = max_pos;
       }
    }
    SubStringFinder(std::string &s, size_t *m, size_t *p) :
        str(s), max_array(m), pos_array(p) { }
};
#endif

int main(int, char *argv[])
{
#if 0
    std::string str[N] = { std::string("a"), std::string("b") };
    for (size_t i = 2; i < N; ++i) str[i] = str[i-1]+str[i-2];
    std::string &to_scan = str[N-1];
    size_t num_elem = to_scan.size();

    size_t *max = new size_t[num_elem];
    size_t *pos = new size_t[num_elem];

    tbb::parallel_for(tbb::blocked_range<size_t>(0, num_elem ), SubStringFinder( to_scan, max, pos ) );

    for (size_t i = 0; i < num_elem; ++i)
        std::cout << " " << max[i] << "(" << pos[i] << ")" << std::endl;
    delete[] pos;
    delete[] max;

    return 0;
#endif
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

#if 0
template <typename... Components>
void conditional_exec (entity_type entity, std::function<void(Components...)> func) {

}

struct CanJump {

};

struct Character {

};

struct Position {

};

struct InputAction {

};

struct SystemBase {
    virtual void apply_tick() {}
    void tick () {
        apply_tick();
    }
};

template <typename StateT, typename... Components>
struct System : SystemBase {
private:
    void apply_tick () {
        StateT& state = static_cast<StateT&>(*this);
        StateT::before_tick(state);
        auto view = registry.view<Components...>();
        view.each(StateT::tick);
        StateT::after_tick(state);
    }
};

struct CharacterSystem : System<CharacterSystem, Character, Position, InputAction> {
    static void before_tick (CharacterSystem& state) {

    }
    static void tick_entity (CharacterSystem& state)
};

void test () {
    conditional_exec<CanJump>(entity, [](auto& jump){

    });

    system<CharacterSystem>();
}
#endif
