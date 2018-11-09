cmake_minimum_required (VERSION 3.8)

project ("impacto")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(ZLIB REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OggVorbis REQUIRED)
find_package(Libatrac9 REQUIRED)

set(Impacto_Src
    src/main.cpp
    src/log.cpp
    src/shader.cpp
    src/util.cpp
    src/window.cpp
    src/workqueue.cpp
    src/game.cpp
    src/modelviewer.cpp
    src/renderer2d.cpp
    src/text.cpp
    src/inputsystem.cpp

    src/text_rne.cpp

    src/nuklear_impl.cpp

    src/3d/camera.cpp
    src/3d/model.cpp
    src/3d/character3d.cpp
    src/3d/background3d.cpp
    src/3d/transform.cpp
    src/3d/scene.cpp
    src/3d/animation.cpp
    src/3d/modelanimator.cpp

    src/io/io.cpp
    src/io/vfs.cpp
    src/io/mpkdriver.cpp
    src/io/cpkdriver.cpp

    src/texture/texture.cpp
    src/texture/s3tc.cpp
    src/texture/gxtloader.cpp
    src/texture/plainloader.cpp

    src/vm/vm.cpp
    src/vm/expression.cpp
    src/vm/thread.cpp
    src/vm/gamespecific_rne.cpp
    src/vm/inst_system.cpp
    src/vm/inst_controlflow.cpp
    src/vm/inst_dialogue.cpp
    src/vm/inst_gamespecific.cpp
    src/vm/inst_graphics2d.cpp
    src/vm/inst_graphics3d.cpp
    src/vm/inst_misc.cpp
    src/vm/inst_movie.cpp
    src/vm/inst_sound.cpp

    src/audio/audiosystem.cpp
    src/audio/audiochannel.cpp
    src/audio/audiostream.cpp
    src/audio/vorbisaudiostream.cpp
    src/audio/atrac9audiostream.cpp
    src/audio/adxaudiostream.cpp
    src/audio/hcaaudiostream.cpp

    vendor/glad/src/glad.c

    vendor/clHCA/clHCA.c
)

set(Impacto_Header
    src/impacto.h
    src/log.h
    src/shader.h
    src/util.h
    src/window.h
    src/workqueue.h
    src/game.h
    src/modelviewer.h
    src/spritesheet.h
    src/renderer2d.h
    src/text.h
    src/loadable.h
    src/inputsystem.h

    src/3d/camera.h
    src/3d/model.h
    src/3d/character3d.h
    src/3d/background3d.h
    src/3d/transform.h
    src/3d/scene.h
    src/3d/animation.h
    src/3d/modelanimator.h

    src/io/io.h
    src/io/vfs.h
    src/io/mpkdriver.h
    
    src/texture/texture.h
    src/texture/s3tc.h
    src/texture/gxtloader.h
    src/texture/plainloader.h

    src/vm/vm.h
    src/vm/expression.h
    src/vm/thread.h
    src/vm/gamespecific_rne.h
    src/vm/inst_macros.inc
    src/vm/inst_system.h
    src/vm/inst_controlflow.h
    src/vm/inst_dialogue.h
    src/vm/inst_gamespecific.h
    src/vm/inst_graphics2d.h
    src/vm/inst_graphics3d.h
    src/vm/inst_misc.h
    src/vm/inst_movie.h
    src/vm/inst_sound.h
    src/vm/opcodetables_rne.h

    src/audio/audiosystem.h
    src/audio/audiocommon.h
    src/audio/audiochannel.h
    src/audio/audiostream.h
    src/audio/buffering.h
    src/audio/vorbisaudiostream.h
    src/audio/atrac9audiostream.h
    src/audio/adxaudiostream.h
    src/audio/hcaaudiostream.h

    vendor/nuklear/nuklear.h
    vendor/nuklear/nuklear_sdl_gl3.h

    vendor/clHCA/clHCA.h
)

include_directories(vendor/glad/include)
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(${OGGVORBIS_INCLUDE_DIRS})
include_directories(${LIBATRAC9_INCLUDE_DIR})
include_directories(vendor/include)

add_executable (impacto ${Impacto_Src} ${Impacto_Header})

# configuration

if(MSVC)
    add_definitions(-DBETTER_ENUMS_NO_CONSTEXPR) # broken in MSVC 2017 apparently
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT ON)
    set(IMPACTO_GL_DEBUG_DEFAULT ON)
else()
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT OFF)
    set(IMPACTO_GL_DEBUG_DEFAULT OFF)
endif()

option(IMPACTO_ENABLE_SLOW_LOG
    "Compile log statements that get hit very frequently or are in a hot path"
    ${IMPACTO_ENABLE_SLOW_LOG_DEFAULT})
option(IMPACTO_GL_DEBUG
    "Use an OpenGL debug context and log messages"
    ${IMPACTO_GL_DEBUG_DEFAULT})

configure_file(src/config.h.in ${PROJECT_BINARY_DIR}/include/config.h)
target_include_directories(impacto PRIVATE ${PROJECT_BINARY_DIR}/include)

# external libs

target_link_libraries(impacto PUBLIC
    ${CMAKE_DL_LIBS}
    SDL2::SDL2main
    glm
    ZLIB::ZLIB
    ${OPENAL_LIBRARY}
    ${OGGVORBIS_LIBRARIES}
    ${LIBATRAC9_LIBRARY}
)

install(TARGETS impacto RUNTIME DESTINATION .)

if (WIN32)
    get_property(SDL2_RUNTIME_DLL TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION)
    install(FILES ${SDL2_RUNTIME_DLL} DESTINATION .)

    install(FILES ${ZLIB_INCLUDE_DIR}/../bin/zlib.dll DESTINATION .)

    install(FILES ${LIBATRAC9_INCLUDE_DIR}/../bin/libatrac9.dll DESTINATION .)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        install(FILES ${OPENAL_INCLUDE_DIR}/../../bin/Win64/soft_oal.dll DESTINATION . RENAME OpenAL32.dll)
    else()
        install(FILES ${OPENAL_INCLUDE_DIR}/../../bin/Win32/soft_oal.dll DESTINATION . RENAME OpenAL32.dll)
    endif()
endif()

# assets

install(DIRECTORY src/shaders DESTINATION .)