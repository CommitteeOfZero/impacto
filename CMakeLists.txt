cmake_minimum_required (VERSION 3.8)

project ("impacto")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

set(Impacto_Src
    src/main.cpp
    src/log.cpp
    src/io/vfs.cpp
    src/camera.cpp
)

set(Impacto_Header
    src/impacto.h
    src/log.h
    src/io/io.h
    src/io/vfs.h
    src/io/vfsdriver.h
    src/camera.h
)

add_executable (impacto ${Impacto_Src} ${Impacto_Header})

# configuration

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT ON)
else()
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT OFF)
endif()

option(IMPACTO_ENABLE_SLOW_LOG
    "Compile log statements that get hit very frequently or are in a hot path"
    ${IMPACTO_ENABLE_SLOW_LOG_DEFAULT})

configure_file(src/config.h.in ${PROJECT_BINARY_DIR}/include/config.h)
target_include_directories(impacto PRIVATE ${PROJECT_BINARY_DIR}/include)

# external libs

target_link_libraries(impacto PUBLIC
    OpenGL::GL
    GLEW::GLEW
    SDL2::SDL2main
    glm
)

install(TARGETS impacto RUNTIME DESTINATION .)

if (WIN32)
    get_property(SDL2_RUNTIME_DLL TARGET SDL2::SDL2 PROPERTY IMPORTED_LOCATION)
    install(FILES ${SDL2_RUNTIME_DLL} DESTINATION .)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        install(FILES ${GLEW_INCLUDE_DIR}/../bin/Release/x64/glew32.dll DESTINATION .)
	else()
        install(FILES ${GLEW_INCLUDE_DIR}/../bin/Release/Win32/glew32.dll DESTINATION .)
	endif()
endif()