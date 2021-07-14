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
    src/mask2d.cpp
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

    src/profile/ui/systemmenu.cpp
    src/profile/ui/titlemenu.cpp
    src/profile/ui/savemenu.cpp
    src/profile/ui/sysmesbox.cpp
    src/profile/ui/selectionmenu.cpp

    src/profile/games/rne/tilebackground.cpp
    src/profile/games/rne/systemmenu.cpp
    src/profile/games/rne/titlemenu.cpp
    src/profile/games/rne/sysmesbox.cpp

    src/profile/games/dash/titlemenu.cpp

    src/profile/games/chlcc/titlemenu.cpp
    src/profile/games/chlcc/sysmesbox.cpp

    src/profile/games/mo6tw/dialoguebox.cpp
    src/profile/games/mo6tw/sysmesbox.cpp
    src/profile/games/mo6tw/systemmenu.cpp
    src/profile/games/mo6tw/titlemenu.cpp
    src/profile/games/mo6tw/savemenu.cpp

    src/profile/games/darling/sysmesbox.cpp

    src/games/rne/tilebackground.cpp
    src/games/rne/datedisplay.cpp
    src/games/rne/systemmenu.cpp
    src/games/rne/titlemenu.cpp
    src/games/rne/sysmesbox.cpp

    src/games/dash/titlemenu.cpp

    src/games/chlcc/titlemenu.cpp
    src/games/chlcc/sysmesbox.cpp
    src/games/chlcc/savesystem.cpp

    src/games/mo6tw/dialoguebox.cpp
	src/games/mo6tw/sysmesbox.cpp
    src/games/mo6tw/systemmenu.cpp
    src/games/mo6tw/titlemenu.cpp
    src/games/mo6tw/savemenu.cpp
    src/games/mo6tw/savesystem.cpp

	src/games/darling/sysmesbox.cpp

    src/hud/dialoguebox.cpp
    src/hud/datedisplay.cpp
    src/hud/saveicondisplay.cpp
    src/hud/loadingdisplay.cpp
    src/hud/waiticondisplay.cpp

    src/ui/ui.cpp
    src/ui/widget.cpp
    src/ui/widgetgroup.cpp
    src/ui/nullmenu.cpp
    src/ui/selectionmenu.cpp
    src/ui/sysmesbox.cpp
    src/ui/widgets/label.cpp
    src/ui/widgets/button.cpp
    src/ui/widgets/mo6tw/titlebutton.cpp
    src/ui/widgets/mo6tw/saveentrybutton.cpp
    src/ui/widgets/chlcc/titlebutton.cpp
    src/ui/widgets/rne/sysmenubutton.cpp

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
    src/texture/ddsloader.cpp

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

    src/video/videosystem.cpp
    src/video/videoplayer.cpp
    src/video/yuvframe.cpp
    src/video/ffmpegstream.cpp

    vendor/glad/src/glad.c

    vendor/clHCA/clHCA.c

    vendor/duktape/src/duktape.c

    vendor/pcg/src/pcg_basic.c
    
    vendor/squish/alpha.cpp 
    vendor/squish/clusterfit.cpp
    vendor/squish/colourblock.cpp 
    vendor/squish/colourfit.cpp 
    vendor/squish/colourset.cpp
    vendor/squish/maths.cpp 
    vendor/squish/rangefit.cpp 
    vendor/squish/singlecolourfit.cpp
    vendor/squish/squish.cpp
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
    src/mask2d.h
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

    src/profile/ui/systemmenu.h
    src/profile/ui/titlemenu.h
    src/profile/ui/savemenu.h
    src/profile/ui/sysmesbox.h
    src/profile/ui/selectionmenu.h

    src/profile/games/rne/tilebackground.h
    src/profile/games/rne/systemmenu.h
    src/profile/games/rne/titlemenu.h
    src/profile/games/rne/sysmesbox.h

    src/profile/games/dash/titlemenu.h

    src/profile/games/chlcc/titlemenu.h
    src/profile/games/chlcc/sysmesbox.h

    src/profile/games/mo6tw/dialoguebox.h
    src/profile/games/mo6tw/sysmesbox.h
    src/profile/games/mo6tw/systemmenu.h
    src/profile/games/mo6tw/titlemenu.h
    src/profile/games/mo6tw/savemenu.h

    src/profile/games/darling/sysmesbox.h

    src/games/rne/tilebackground.h
    src/games/rne/datedisplay.h
    src/games/rne/systemmenu.h
    src/games/rne/titlemenu.h
    src/games/rne/sysmesbox.h

    src/games/dash/titlemenu.h

    src/games/chlcc/titlemenu.h
    src/games/chlcc/sysmesbox.h
    src/games/chlcc/savesystem.h

    src/games/mo6tw/dialoguebox.h
    src/games/mo6tw/sysmesbox.h
    src/games/mo6tw/systemmenu.h
    src/games/mo6tw/titlemenu.h
    src/games/mo6tw/savemenu.h
    src/games/mo6tw/savesystem.h

    src/games/darling/sysmesbox.h

    src/hud/dialoguebox.h
    src/hud/datedisplay.h
    src/hud/saveicondisplay.h
    src/hud/loadingdisplay.h
    src/hud/waiticondisplay.h

    src/ui/ui.h
    src/ui/widgetgroup.h
    src/ui/menu.h
    src/ui/nullmenu.h
    src/ui/selectionmenu.h
    src/ui/sysmesbox.h
    src/ui/widget.h
    src/ui/widgets/label.h
    src/ui/widgets/button.h
    src/ui/widgets/mo6tw/titlebutton.h
    src/ui/widgets/mo6tw/saveentrybutton.h
    src/ui/widgets/chlcc/titlebutton.h
    src/ui/widgets/rne/sysmenubutton.h

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

    src/video/videosystem.h
    src/video/videoplayer.h
    src/video/yuvframe.h
    src/video/ffmpegstream.h

    vendor/nuklear/nuklear.h
    vendor/nuklear/nuklear_sdl_gl3.h
    
    vendor/clHCA/clHCA.h
    
    vendor/squish/squish.h
)

if (WIN32)
    list(APPEND Impacto_Src
        src/manifest_windows.manifest
    )
endif()

# dependencies

if (WIN32)
    # Workaround for RelWithDebInfo builds not installing all the libraries
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

    set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
    set(LIBATRAC9_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/LibAtrac9/include")

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LIBATRAC9_ARCH "x64")
    else()
        set(LIBATRAC9_ARCH "x86")
    endif()

    set(LIBATRAC9_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/LibAtrac9/bin/${LIBATRAC9_ARCH}/libatrac9.lib")
endif()

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
    find_package(FFMPEG REQUIRED)

    include_directories(${OGGVORBIS_INCLUDE_DIRS})

    list(APPEND Impacto_Libs
        ${FFMPEG_LIBRARIES}
        ${SDL2_LIBRARIES}
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
    find_package(FFMPEG REQUIRED)

    include_directories(${OGGVORBIS_INCLUDE_DIRS})

    # Add this because apparently find_package(SDL2 REQUIRED) doesn't work right on Arch?
    if("${SDL2_LIBRARIES}" STREQUAL "")
        set(SDL2_LIBRARIES "SDL2::SDL2")
    endif()

    list(APPEND Impacto_Libs
        ${FFMPEG_LIBRARIES}
        ${SDL2_LIBRARIES}
        ZLIB::ZLIB
        ${OGGVORBIS_LIBRARIES}
    )

    if(VCPKG_TOOLCHAIN)
        list(APPEND Impacto_Libs
            glm::glm
        )
    elseif(NOT APPLE) # If we don't do this it decides to link glm as a library on macOS for some reason        
        list(APPEND Impacto_Libs
            glm
        )
    endif()

    add_definitions(-DIMPACTO_OPENAL_HAVE_ALEXT)
endif()

include_directories(vendor/glad/include)
include_directories(${LIBATRAC9_INCLUDE_DIR})
include_directories(${OPENAL_INCLUDE_DIR})
include_directories(${FFMPEG_INCLUDE_DIRS})
include_directories(${SDL2_INCLUDE_DIRS})
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
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION .
        FILES_MATCHING PATTERN "*.dll"
        PATTERN ".cmake" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN "include" EXCLUDE
        PATTERN "Testing" EXCLUDE
    )
    install(FILES ${LIBATRAC9_INCLUDE_DIR}/../bin/${LIBATRAC9_ARCH}/libatrac9.dll DESTINATION .)
endif()

# asset install

install(DIRECTORY src/shaders DESTINATION .)
install(DIRECTORY profiles DESTINATION .)
install(DIRECTORY games DESTINATION .)