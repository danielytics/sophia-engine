TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += include \
               depends/moodycamel/include \
               depends/yaml-cpp/include \
               depends/glm-0.9.7.4/include \
			   depends/spdlog/include \
			   depends/entt/src

QMAKE_CXXFLAGS_RELEASE += -O3 -msse4.1 -mssse3 -msse3 -msse2 -msse2 -DGLM_FORCE_INLINE -DSPDLOG_NO_THREAD_ID -DSPDLOG_NO_NAME
QMAKE_CXXFLAGS_DEBUG += -DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON -DSPDLOG_NO_THREAD_ID -DSPDLOG_NO_NAME
#QMAKE_CXXFLAGS_DEBUG += -O3 -msse4.1 -mssse3 -msse3 -msse2 -msse2 -DGLM_FORCE_INLINE

# Platform-specific configuration
#################################


# Mac OS X
macx {
    QMAKE_LFLAGS += -pagezero_size 10000 -image_base 100000000
	INCLUDEPATH += /usr/local/Cellar/sdl2/2.0.8/include \
				   /usr/local/Cellar/glew/2.1.0/include \
                   /usr/local/include/luajit-2.0
    LIBS += -framework OpenGL \
			-L/usr/local/Cellar/sdl2/2.0.8/lib -lSDL2 \
			-L/usr/local/Cellar/glew/2.1.0/lib -lGLEW \
			/usr/local/lib/libyaml-cpp.a \
			/usr/local/lib/libluajit-5.1.a
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

DISTFILES += \
    data/shaders/pbr.frag \
    data/shaders/pbr.vert \
    data/shaders/lamp.frag \
    data/shaders/lighting.frag \
    data/shaders/lighting.vert \
    data/shaders/shadowmap.frag \
    data/shaders/shadowmap.vert

SOURCES += src/core/main.cpp \
    src/util/telemetry.cpp \
    src/util/config.cpp \
    src/util/logging.cpp \
    src/window/window.cpp \
    src/graphics/shader.cpp \
    src/graphics/tilemap.cpp \
    src/graphics/spritepool.cpp

HEADERS += \
    include/util/telemetry.h \
    include/util/config.h \
    include/util/logging.h \
    include/window/window.h \
    include/graphics/shader.h \
    include/util/stb_image.h \
    include/graphics/tilemap.h \
    include/graphics/mesh.h \
    include/graphics/renderable.h \
    include/graphics/spritepool.h
