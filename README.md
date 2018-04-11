
# Sophia Game Engine

The Sophia Game Engine is a 2D-first game engine written in C++14.

# Design Goals

Sophia is being designed for 2D games (side-scroller and top-down, no plans for isometric for now), but unlike many other 2D game engines, Sophia is designed with the expectation that sometimes games may want 3D background, 3D character models or 3D physics (top-down with jumping/stacking is a major use-case).
Additionally, Sophia aims to combine the benefits of an Entity Component System (modularity and performance) with the convenience of a tree hierarchy of game objects. Sophia also aims to have an editor that features a consistent and sensible, easy to use workflow.

So: flexible, performant good UX. All while open source.

Obviously, there is a long long way to go to realise these goals.

# Dependencies

Engine Dependencies:

* OpenGL 4.1
* [EnTT](https://github.com/skypjack/entt) - Entity Component System (MIT License)
* [SDL2](http://libsdl.org/) - Windowing & Input (ZLIB License)
* [GLEW](http://glew.sourceforge.net/) - OpenGL extension library (Modified BSD & MIT Licenses)
* [moodycamel::ConcurrentQueue](https://github.com/cameron314/concurrentqueue) - lock-free queue (Simplified BSD License)
* [YAML-cpp](https://github.com/jbeder/yaml-cpp) - YAML parser (MIT License)
* [GLM](https://glm.g-truc.net/0.9.8/index.html) - OpenGL Mathematics library (The Happy Bunny/Modified MIT License)
* [spdlog](https://github.com/gabime/spdlog) - Fast logging library (MIT License)
* [LuaJIT](http://luajit.org/luajit.html) - Lua bindings and JIT runtime (MIT License)
* [Assimp](http://assimp.org/) - Asset importer (BSD License)
* [Bullet](https://github.com/bulletphysics/bullet3) - Physics engine (zlib License)

Editor Dependencies:

* [Qt](https://www.qt.io/developers/) - GUI framework (Lesser General Public License v3)

# Visual Scripting

Ever since working with Max/MSP many years ago, I've felt that visual programming is an amazing way of writing some types of programs, but (at least at the time), Max was too limited to be of general purpose use. Other game engines have demonstrated the usefulness and ease-of-use of visual scripting, but their programming models were never satisfactory to me. The goal with Sophia's visual scripting is to take the best bits of Max, sprinkle in my own pixie dust and make it a first-class citizen and viable means of developing Sophia-based games.
The goal is that the visual language will be interpreted during development, but translated to C (and compiled into a shared object) during export, providing both ease of development and good runtime integration & performance.

Lua is also planned to be supported, for those that prefer textual programming but do not wish to use C++.

As with the design goals above, there is obviously still a loooong way to go before these goals are realised.

# License

The Sophia game engine and all of its tools are released under the terms of the [MIT License](https://github.com/danielytics/sophia/blob/master/LICENSE).

