
# Sophia Game Engine

The Sophia Game Engine is a 2D-first game engine written in C++14.

Development is happening in branches.

# Design Goals

Sophia takes ideas and influences from many things and places:

* [Godot's scene tree](http://docs.godotengine.org/en/3.0/getting_started/step_by_step/scenes_and_nodes.html)
* [Unity's new Entity Component System](https://github.com/Unity-Technologies/EntityComponentSystemSamples/blob/master/Documentation/index.md)
* [Octopath Traveller](https://en.m.wikipedia.org/wiki/Octopath_Traveler)'s graphics style
* [Artificial Intelligence for Games](https://www.amazon.com/Artificial-Intelligence-Games-Ian-Millington/dp/0123747317/ref=cm_cr_arp_d_product_top?ie=UTF8), [GameAI Pro](http://www.gameaipro.com) and [Left4Dead's AI Director](https://aiandgames.com/in-the-directors-chair-left-4-dead/)

Sophia aims to combine concepts and ideas from the above influences into a coherent, flexible, yet performant whole. To this end, Sophia aims to combine a high-performance and flexible Entity Component System (based on [EnTT](https://github.com/skypjack/entt)) with the convenience of a tree hierarchy of game objects, with all of the supporting logic and systems for powerful AI (not just for controlling NPC's, but also for controlling game flow and narrative), physics and editing tools (most game data is described in YAML files, making it easy to both edit by hand and read/write by tooling). The renderer is designed to be relatively modern (either deferred rendering or forward+ or a hybrid, we will see), using modern OpenGL (4.1), hand-crafted for an Octopath Traveller-inspired 2.5D fixed-camera-3D environment with 2D billboarded sprite characters and objects. 

On the tooling front, Sophia aims to have a straightforward and consistent workflow, with a good, consistent user experience. UX is very important and shouldn't be an afterthought.

The goal is to support Windows, Linux and Mac (possibly iOS in the future, although this would require either an OpenGL ES or Metal renderer).

All with an open source license that doesn't restrict what you can do.


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

