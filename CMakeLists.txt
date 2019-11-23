cmake_minimum_required (VERSION 3.8)

project ("impacto")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

if(EMSCRIPTEN)
    list(APPEND CMAKE_MODULE_PATH ${EMSCRIPTEN_ROOT_PATH}/cmake/Modules)
endif()

set(Impacto_Src
    src/main.cpp
    src/log.cpp
    src/shader.cpp
    src/util.cpp
    src/window.cpp
    src/workqueue.cpp
    src/game.cpp
    src/mem.cpp
    src/modelviewer.cpp
    src/characterviewer.cpp
    src/spriteanimation.cpp
    src/renderer2d.cpp
    src/background2d.cpp
    src/character2d.cpp
    src/text.cpp
    src/inputsystem.cpp
    src/savesystem.cpp
    src/glc.cpp

    src/profile/profile.cpp
    src/profile/profile_internal.cpp
    src/profile/game.cpp
    src/profile/vfs.cpp
    src/profile/scene3d.cpp
    src/profile/sprites.cpp
    src/profile/animations.cpp
    src/profile/charset.cpp
    src/profile/fonts.cpp
    src/profile/dialogue.cpp
    src/profile/vm.cpp
    src/profile/scriptvars.cpp
	src/profile/scriptinput.cpp
    src/profile/savesystem.cpp

    src/profile/hud/saveicon.cpp
    src/profile/hud/loadingdisplay.cpp
    src/profile/hud/datedisplay.cpp
    src/profile/hud/mainmenu.cpp
    src/profile/hud/titlemenu.cpp
    src/profile/hud/sysmesbox.cpp
    src/profile/hud/selectiondisplay.cpp

    src/profile/games/rne/tilebackground.cpp
    src/profile/games/rne/mainmenu.cpp
    src/profile/games/rne/titlemenu.cpp
    src/profile/games/rne/sysmesbox.cpp

    src/profile/games/dash/titlemenu.cpp

    src/profile/games/chlcc/titlemenu.cpp
    src/profile/games/chlcc/sysmesbox.cpp

    src/profile/games/mo6tw/sysmesbox.cpp

    src/profile/games/darling/sysmesbox.cpp

    src/games/rne/tilebackground.cpp
    src/games/rne/datedisplay.cpp
    src/games/rne/mainmenu.cpp
    src/games/rne/titlemenu.cpp
    src/games/rne/sysmesbox.cpp

    src/games/dash/titlemenu.cpp

    src/games/chlcc/titlemenu.cpp
    src/games/chlcc/sysmesbox.cpp
    src/games/chlcc/savesystem.cpp

	src/games/mo6tw/sysmesbox.cpp

	src/games/darling/sysmesbox.cpp

    src/hud/datedisplay.cpp
    src/hud/saveicondisplay.cpp
    src/hud/sysmesbox.cpp
    src/hud/loadingdisplay.cpp
    src/hud/mainmenu.cpp
    src/hud/titlemenu.cpp
    src/hud/waiticondisplay.cpp
    src/hud/selectiondisplay.cpp

    src/nuklear_impl.cpp
    src/stbi_impl.c

    src/3d/camera.cpp
    src/3d/model.cpp
    src/3d/renderable3d.cpp
    src/3d/transform.cpp
    src/3d/scene.cpp
    src/3d/animation.cpp
    src/3d/modelanimator.cpp

    src/io/vfs.cpp
    src/io/assetpath.cpp
    src/io/memorystream.cpp
    src/io/physicalfilestream.cpp
    src/io/uncompressedstream.cpp
    src/io/zlibstream.cpp
    src/io/vfsarchive.cpp
    src/io/mpkarchive.cpp
    src/io/cpkarchive.cpp
    src/io/lnk4archive.cpp
    src/io/textarchive.cpp
    src/io/afsarchive.cpp

    src/texture/texture.cpp
    src/texture/s3tc.cpp
    src/texture/bcdecode.cpp
    src/texture/bntxloader.cpp
    src/texture/gxtloader.cpp
    src/texture/plainloader.cpp
    src/texture/stbiloader.cpp

    src/vm/vm.cpp
    src/vm/expression.cpp
    src/vm/thread.cpp
    src/vm/inst_system.cpp
    src/vm/inst_controlflow.cpp
    src/vm/inst_dialogue.cpp
    src/vm/inst_gamespecific.cpp
    src/vm/inst_graphics2d.cpp
    src/vm/inst_graphics3d.cpp
    src/vm/inst_misc.cpp
    src/vm/inst_movie.cpp
    src/vm/inst_sound.cpp

    src/vm/interface/scene3d.cpp
    src/vm/interface/scene2d.cpp
    src/vm/interface/input.cpp

    src/audio/audiosystem.cpp
    src/audio/audiochannel.cpp
    src/audio/audiostream.cpp
    src/audio/vorbisaudiostream.cpp
    src/audio/atrac9audiostream.cpp
    src/audio/adxaudiostream.cpp
    src/audio/hcaaudiostream.cpp

    vendor/glad/src/glad.c

    vendor/clHCA/clHCA.c

    vendor/duktape/src/duktape.c

    vendor/pcg/src/pcg_basic.c
)

set(Impacto_Header
    src/impacto.h
    src/log.h
    src/shader.h
    src/util.h
    src/window.h
    src/workqueue.h
    src/game.h
    src/mem.h
    src/modelviewer.h
    src/characterviewer.h
    src/spritesheet.h
    src/spriteanimation.h
    src/font.h
    src/renderer2d.h
    src/background2d.h
    src/character2d.h
    src/text.h
    src/loadable.h
    src/inputsystem.h
    src/savesystem.h
    src/glc.h
    src/rng.h
    src/animation.h

    src/profile/profile.h
    src/profile/profile_internal.h
    src/profile/game.h
    src/profile/vfs.h
    src/profile/scene3d.h
    src/profile/sprites.h
    src/profile/animations.h
    src/profile/charset.h
    src/profile/fonts.h
    src/profile/dialogue.h
    src/profile/vm.h
    src/profile/scriptvars.h
    src/profile/scriptinput.h
    src/profile/savesystem.h

    src/profile/hud/saveicon.h
    src/profile/hud/loadingdisplay.h
    src/profile/hud/datedisplay.h
    src/profile/hud/mainmenu.h
    src/profile/hud/titlemenu.h
    src/profile/hud/sysmesbox.h
    src/profile/hud/selectiondisplay.h

    src/profile/games/rne/tilebackground.h
    src/profile/games/rne/mainmenu.h
    src/profile/games/rne/titlemenu.h
    src/profile/games/rne/sysmesbox.h

    src/profile/games/dash/titlemenu.h

    src/profile/games/chlcc/titlemenu.h
    src/profile/games/chlcc/sysmesbox.h

    src/profile/games/mo6tw/sysmesbox.h

    src/profile/games/darling/sysmesbox.h

    src/games/rne/tilebackground.h
    src/games/rne/datedisplay.h
    src/games/rne/mainmenu.h
    src/games/rne/titlemenu.h
    src/games/rne/sysmesbox.h

    src/games/dash/titlemenu.h

    src/games/chlcc/titlemenu.h
    src/games/chlcc/sysmesbox.h
    src/games/chlcc/savesystem.h

    src/games/mo6tw/sysmesbox.h

    src/games/darling/sysmesbox.h

    src/hud/datedisplay.h
    src/hud/saveicondisplay.h
    src/hud/sysmesbox.h
    src/hud/loadingdisplay.h
    src/hud/mainmenu.h
    src/hud/titlemenu.h
    src/hud/waiticondisplay.h
    src/hud/selectiondisplay.h

    src/3d/camera.h
    src/3d/model.h
    src/3d/renderable3d.h
    src/3d/transform.h
    src/3d/scene.h
    src/3d/animation.h
    src/3d/modelanimator.h

    src/io/io.h
    src/io/vfs.h
    src/io/buffering.h
    src/io/filemeta.h
    src/io/assetpath.h
    src/io/inputstream.h
    src/io/vfsarchive.h
    src/io/memorystream.h
    src/io/physicalfilestream.h
    src/io/uncompressedstream.h
    src/io/zlibstream.h

    src/texture/texture.h
    src/texture/s3tc.h
    src/texture/bcdecode.h
    src/texture/gxtloader.h
    src/texture/bntxloader.h
    src/texture/plainloader.h

    src/vm/vm.h
    src/vm/expression.h
    src/vm/thread.h
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

    src/vm/interface/scene3d.h
    src/vm/interface/scene2d.h
	src/vm/interface/input.h

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

if (WIN32)
    list(APPEND Impacto_Src
        src/manifest_windows.manifest
    )
endif()

# dependencies

find_package(Libatrac9 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenAL REQUIRED)

if(NX) # avoid CMAKE_DL_LIBS for NX
set(Impacto_Libs
    ${LIBATRAC9_LIBRARY}
    ${OPENAL_LIBRARY}
)
else()
set(Impacto_Libs
    ${CMAKE_DL_LIBS}
    ${LIBATRAC9_LIBRARY}
    ${OPENAL_LIBRARY}
)
endif()

if(EMSCRIPTEN)
    # BINARYEN_TRAP_MODE=clamp => https://groups.google.com/forum/#!topic/emscripten-discuss/IJr4ApiW_zU
    # duk_heap_alloc() errors without this
    # MAIN_MODULE=1 => SDL_GL_GetProcAddress falls back to dlsym() so we need dynamic linking support (...)

    set(IMPACTO_EMSCRIPTEN_BUILD_FLAGS "\
        -s USE_SDL=2 \
        -s USE_ZLIB=1 \
        -s USE_OGG=1 \
        -s USE_VORBIS=1 \
        -s WASM=1 \
        -s USE_WEBGL2=1 \
        -s MAIN_MODULE=2 \
        -s BINARYEN_TRAP_MODE=clamp \
        -s ALLOW_MEMORY_GROWTH=1 \
        -g4 \
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/profiles@/profiles \
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/games@/games \
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders@/shaders \
    ")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(IMPACTO_EMSCRIPTEN_BUILD_FLAGS "${IMPACTO_EMSCRIPTEN_BUILD_FLAGS} \
            -s GL_ASSERTIONS=1 \
            -s GL_DEBUG=1 \
        ")
    else()
        set(IMPACTO_EMSCRIPTEN_BUILD_FLAGS "${IMPACTO_EMSCRIPTEN_BUILD_FLAGS} \
            -O3 \
        ")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")

    string(REPLACE "-O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "-O2" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REPLACE "-O2" "" CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

    add_definitions(-DIMPACTO_OPENAL_INCLUDE_SUBDIR)
elseif(NX)
    find_package(SDL2 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(OggVorbis REQUIRED)

    include_directories(${OGGVORBIS_INCLUDE_DIRS})

    list(APPEND Impacto_Libs
        SDL2::SDL2main
        glm
        ZLIB::ZLIB
        vorbisfile
        vorbis
        ogg
        EGL
        glapi
        drm_nouveau
        nx
    )

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group") # workaround for SDL2 being last in the linker list

    add_definitions(-DIMPACTO_OPENAL_INCLUDE_SUBDIR)
else()
    find_package(SDL2 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(OggVorbis REQUIRED)

    include_directories(${OGGVORBIS_INCLUDE_DIRS})

    list(APPEND Impacto_Libs
        SDL2::SDL2main
        glm
        ZLIB::ZLIB
        ${OGGVORBIS_LIBRARIES}
    )

    add_definitions(-DIMPACTO_OPENAL_HAVE_ALEXT)
endif()

include_directories(vendor/glad/include)
include_directories(${LIBATRAC9_INCLUDE_DIR})
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(vendor/include)
include_directories(vendor/duktape/include)
include_directories(vendor/pcg/include)

add_executable (impacto ${Impacto_Src} ${Impacto_Header})

target_link_libraries(impacto PUBLIC ${Impacto_Libs})

# compiler/dependency configuration

set_property(TARGET impacto PROPERTY CXX_STANDARD 14)

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

if(MSVC)
    add_definitions(-DBETTER_ENUMS_NO_CONSTEXPR) # broken in MSVC 2017 apparently
endif()

if(NX)
    set_property(TARGET impacto PROPERTY NO_SYSTEM_FROM_IMPORTED ON) # to avoid -isystem
    add_definitions(-DENV64BIT)
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DENV64BIT)
    else()
        add_definitions(-DENV32BIT)
    endif()
endif()

add_definitions(-DGLM_FORCE_RADIANS) # lol

# target configuration

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

if(EMSCRIPTEN)
    set(IMPACTO_HAVE_THREADS OFF)
    set(IMPACTO_USE_SDL_HIGHDPI ON)
else()
    set(IMPACTO_HAVE_THREADS ON)
    set(IMPACTO_USE_SDL_HIGHDPI OFF)
endif()

configure_file(src/config.h.in ${PROJECT_BINARY_DIR}/include/config.h)
target_include_directories(impacto PRIVATE ${PROJECT_BINARY_DIR}/include)

# binary install

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

# asset install

install(DIRECTORY src/shaders DESTINATION .)
install(DIRECTORY profiles DESTINATION .)
install(DIRECTORY games DESTINATION .)