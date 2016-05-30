
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <concurrentqueue.h>
#include <lua.hpp>

#include "util/telemetry.h"

struct Test {
    int foo;
    int bar;
};

struct Settings {
    int test_1;
    int test_2;
    float test_3;
    std::string test_4;
    std::string test_5;
    std::vector<int> test_6;
    struct {
        int a;
        std::string b;
        std::vector<int> c;
    } test_7;
    std::vector<Test> test_8;
    std::vector<std::vector<int>> test_9;
};

int main(int argc, char *argv[])
{
    auto settings = Settings{};
    {
        Test temp;
        std::vector<int> temp2;
        using namespace Config;
        auto parser = make_parser(
            scalar("test_1", settings.test_1),
            scalar("test_2", settings.test_2),
            [](const YAML::Node& node){
                std::cout << "Extracting raw test_2: " << node["test_2"].as<int>() << "\n";
            },
            scalar("test_3", settings.test_3),
            scalar("test_4", settings.test_4),
            scalar("test_5", settings.test_5),
            sequence("test_6", settings.test_6),
            map("test_7",
                scalar("a", settings.test_7.a),
                scalar("b", settings.test_7.b),
                sequence("c", settings.test_7.c)
            ),
            sequence("test_8",
                temp, settings.test_8,
                scalar("foo", temp.foo),
                scalar("bar", temp.bar)
            ),
            sequence("test_9",
                temp2, settings.test_9,
                sequence(temp2)
            )
        );
        parser(YAML::LoadFile("config.yml"));
    }
    std::cout << "test_1: " << settings.test_1 << "\n";
    std::cout << "test_2: " << settings.test_2 << "\n";
    std::cout << "test_3: " << settings.test_3 << "\n";
    std::cout << "test_4: " << settings.test_4 << "\n";
    std::cout << "test_5: " << settings.test_5 << "\n";
    std::cout << "test_6: ";
    for (auto val : settings.test_6) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    std::cout << "test_7.a: " << settings.test_7.a << "\n";
    std::cout << "test_7.b: " << settings.test_7.b << "\n";
    std::cout << "test_7.c: ";
    for (auto val : settings.test_7.c) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    for (auto val : settings.test_8) {
        std::cout << "test_8: .foo: " << val.foo << ", .bar: " << val.bar << "\n";
    };
    for (auto vals : settings.test_9) {
        std::cout << "test_9: ";
        for (auto val : vals) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    moodycamel::ConcurrentQueue<int> queue;
    queue.enqueue(8);

    int item;
    if (queue.try_dequeue(item)) {
        std::cout << item << "\n";
    } else {
        std::cout << "No items in queue\n";
    }
    return 0;
}

