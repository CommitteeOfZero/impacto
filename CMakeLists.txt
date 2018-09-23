cmake_minimum_required (VERSION 3.8)

project ("impacto")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)
find_package(ZLIB REQUIRED)

set(Impacto_Src
    src/main.cpp
    src/log.cpp
    src/shader.cpp
    src/util.cpp
    src/window.cpp
    src/workqueue.cpp

    src/nuklear_impl.cpp

    src/3d/camera.cpp
    src/3d/model.cpp
    src/3d/character3d.cpp
    src/3d/transform.cpp
    src/3d/scene.cpp
    src/3d/animation.cpp
    src/3d/modelanimator.cpp

    src/io/io.cpp
    src/io/vfs.cpp
    src/io/mpkdriver.cpp

    src/texture/texture.cpp
    src/texture/s3tc.cpp
    src/texture/gxtloader.cpp
)

set(Impacto_Header
    src/impacto.h
    src/log.h
    src/shader.h
    src/util.h
    src/window.h
    src/workqueue.h

    src/3d/camera.h
    src/3d/model.h
    src/3d/character3d.h
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

    vendor/include/flat_hash_map.hpp

    vendor/nuklear/nuklear.h
    vendor/nuklear/nuklear_sdl_gl3.h
)

add_executable (impacto ${Impacto_Src} ${Impacto_Header})

# configuration

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
    OpenGL::GL
    GLEW::GLEW
    SDL2::SDL2main
    glm
    ZLIB::ZLIB
)

install(TARGETS impacto RUNTIME DESTINATION .)

if (WIN32)
    get_property(SDL2_RUNTIME_DLL TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION)
    install(FILES ${SDL2_RUNTIME_DLL} DESTINATION .)

    install(FILES ${ZLIB_INCLUDE_DIR}/../bin/zlib.dll DESTINATION .)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        install(FILES ${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll DESTINATION .)
	else()
        install(FILES ${GLEW_INCLUDE_DIR}/../bin/Release/Win32/glew32.dll DESTINATION .)
	endif()
endif()

# assets

install(DIRECTORY src/shaders DESTINATION .)