TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

# Select modules
#################################
PHYSICS_ENGINE = BULLET
STD_LIB = EASTL # STD
#################################

# General configuration
#################################

INCLUDEPATH += include \
               depends/moodycamel/include \
               depends/yaml-cpp/include \
               depends/glm-0.9.7.4/include \
			   depends/spdlog/include \
			   depends/entt/src \
			   depends/physfs-cpp/include \
			   depends/EASTL/test/packages/EABase/include/Common \ # A bizarre EASTL dependency setup...
			   depends/EASTL/include
#			   depends/assimp-4.1.0/include

QMAKE_CXXFLAGS_RELEASE += -O3 -msse4.1 -mssse3 -msse3 -msse2 -msse2 -DGLM_FORCE_INLINE -DSPDLOG_NO_THREAD_ID -DSPDLOG_NO_NAME
QMAKE_CXXFLAGS_DEBUG += -DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON -DSPDLOG_NO_THREAD_ID -DSPDLOG_NO_NAME -DDEBUG_BUILD
#QMAKE_CXXFLAGS_DEBUG += -O3 -msse4.1 -mssse3 -msse3 -msse2 -msse2 -DGLM_FORCE_INLINE

contains(STD_LIB, EASTL) {
	QMAKE_CXXFLAGS_RELEASE += -DUSE_EASTL
	QMAKE_CXXFLAGS_DEBUG += -DUSE_EASTL
}

# Conditionally add source files depending on selected physics engine
#################################
contains(PHYSICS_ENGINE, BULLET) {
	SOURCES += src/physics/bullet/BulletEngine.cpp
	macx {
		INCLUDEPATH += /usr/local/include/bullet
		LIBS += -L/usr/local/lib/
	}
	LIBS += -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath
}

# Platform-specific configuration
#################################

# Mac OS X
macx {
    QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000
	INCLUDEPATH += /usr/local/Cellar/sdl2/2.0.8/include \
				   /usr/local/Cellar/glew/2.1.0/include \
				   /usr/local/Cellar/physfs/3.0.1/include \
				   /usr/local/include/luajit-2.0 \
				   /usr/local/Cellar/tbb/2018_U3_1/include
	LIBS += -framework OpenGL \
			-L/usr/local/Cellar/sdl2/2.0.8/lib -lSDL2 \
			-L/usr/local/Cellar/glew/2.1.0/lib -lGLEW \
			-L/usr/local/Cellar/physfs/3.0.1/lib -lphysfs \
			-L/usr/local/Cellar/tbb/2018_U3_1/lib -ltbb \
			-L$$PWD/depends/EASTL/build -lEASTL \
			-lyaml-cpp \
			-lluajit-5.1
#			$$PWD/depends/assimp-4.1.0/lib/libassimp.dylib
#            -F/Library/Frameworks -framework SDL2
}

# Windows
win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        # 32bit Windows
        error("32bit is not supported.")
    } else {
        # 64bit Windows

    }
}

# Linux
unix:!macx {

}

# Embedded Dependency Files
#################################
SOURCES += depends/physfs-cpp/src/physfs.cpp \ # Using the static library causes symbol mismatch unless same compiler is used
    src/util/Helpers.cpp \
    src/ecs/Loader.cpp \
    src/graphics/Model.cpp \
    src/ecs/ctors/Transform.cpp

# Project Files
#################################

DISTFILES += \
    data/shaders/pbr.frag \
    data/shaders/pbr.vert \
    data/shaders/lamp.frag \
    data/shaders/lighting.frag \
    data/shaders/lighting.vert \
    data/shaders/shadowmap.frag \
    data/shaders/shadowmap.vert \
    data/shaders/sprites.frag \
    data/shaders/tiles.frag \
    data/shaders/sprites.vert \
    data/shaders/tiles.vert \
    data/shaders/gbuffer.frag \
    data/shaders/gbuffer.vert \
    data/shaders/pbr.frag \
    data/shaders/deferredlighting.vert \
    data/shaders/debug.frag \
    data/shaders/debug.vert \
    data/shaders/background.frag \
    data/shaders/background.vert \
    data/shaders/model.frag \
    data/shaders/model.vert

SOURCES += src/core/main.cpp \
    src/graphics/DeferredRenderer.cpp \
    src/graphics/Shader.cpp \
    src/graphics/SpritePool.cpp \
    src/graphics/TileMap.cpp \
    src/util/Telemetry.cpp \
    src/util/Logging.cpp \
    src/util/Config.cpp \
    src/window/Window.cpp

HEADERS += \
    include/util/stb_image.h \
    include/graphics/DeferredRenderer.h \
    include/graphics/Mesh.h \
    include/graphics/Renderable.h \
    include/graphics/Shader.h \
    include/graphics/SpritePool.h \
    include/graphics/TileMap.h \
    include/util/Config.h \
    include/util/Helpers.h \
    include/util/Logging.h \
    include/util/Telemetry.h \
    include/window/Window.h \
    include/graphics/Debug.h \
    include/world/Scene.h \
    include/math/Types.h \
    include/math/AABB.h \
    include/physics/Engine.h \
    include/ecs/components/TriggerRegion.h \
    include/ecs/components/RigidBody.h \
    include/ecs/components/Sprite.h \
    include/ecs/components/Spawner.h \
    include/ecs/components/Behavior.h \
    include/ecs/components/Global.h \
    include/ecs/components/CharacterController.h \
    include/ecs/Loader.h \
    include/ecs/components/TimeAware.h \
    include/ecs/components/Hierarchy.h \
    include/graphics/Model.h \
    include/lib.h \
    include/ecs/components/AABB.h \
    include/ecs/components/Material.h \
    include/ecs/components/Mesh.h \
    include/ecs/components/Transform.h \
    include/ecs/ctors/Transform.h \
    include/ecs/ctors/Component.h
