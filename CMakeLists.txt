cmake_minimum_required(VERSION 3.8)

project("impacto")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

if (EMSCRIPTEN)
    list(APPEND CMAKE_MODULE_PATH ${EMSCRIPTEN_ROOT_PATH}/cmake/Modules)
endif ()

set(Impacto_Src
        src/main.cpp
        src/log.cpp
        src/util.cpp
        src/workqueue.cpp
        src/game.cpp
        src/mem.cpp
        src/modelviewer.cpp
        src/characterviewer.cpp
        src/spriteanimation.cpp
        src/background2d.cpp
        src/mask2d.cpp
        src/character2d.cpp
        src/text.cpp
        src/inputsystem.cpp
        src/minilua_impl.cpp

        src/renderer/renderer.cpp

        src/data/savesystem.cpp
        src/data/tipssystem.cpp

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

        src/profile/data/savesystem.cpp
        src/profile/data/tipssystem.cpp

        src/profile/hud/saveicon.cpp
        src/profile/hud/loadingdisplay.cpp
        src/profile/hud/datedisplay.cpp
        src/profile/hud/tipsnotification.cpp
        src/profile/hud/delusiontrigger.cpp

        src/profile/ui/systemmenu.cpp
        src/profile/ui/titlemenu.cpp
        src/profile/ui/savemenu.cpp
        src/profile/ui/sysmesbox.cpp
        src/profile/ui/selectionmenu.cpp
        src/profile/ui/backlogmenu.cpp
        src/profile/ui/optionsmenu.cpp
        src/profile/ui/tipsmenu.cpp
        src/profile/ui/extramenus.cpp
        src/profile/ui/trophymenu.cpp
        src/profile/ui/mapsystem.cpp

        src/profile/games/rne/tilebackground.cpp
        src/profile/games/rne/systemmenu.cpp
        src/profile/games/rne/titlemenu.cpp
        src/profile/games/rne/sysmesbox.cpp

        src/profile/games/dash/titlemenu.cpp
        
        src/profile/games/chlcc/dialoguebox.cpp
        src/profile/games/chlcc/titlemenu.cpp
        src/profile/games/chlcc/savemenu.cpp
        src/profile/games/chlcc/sysmesbox.cpp
        src/profile/games/chlcc/clearlistmenu.cpp
        src/profile/games/chlcc/moviemenu.cpp
        src/profile/games/chlcc/albummenu.cpp
        src/profile/games/chlcc/musicmenu.cpp
        src/profile/games/chlcc/tipsmenu.cpp
        src/profile/games/chlcc/optionsmenu.cpp
        src/profile/games/chlcc/systemmenu.cpp
        src/profile/games/chlcc/trophymenu.cpp
        src/profile/games/chlcc/delusiontrigger.cpp

        src/profile/games/cc/dialoguebox.cpp
        src/profile/games/cc/titlemenu.cpp
        src/profile/games/cc/sysmesbox.cpp

        src/profile/games/cclcc/titlemenu.cpp
        src/profile/games/cclcc/savemenu.cpp
        src/profile/games/cclcc/optionsmenu.cpp
        src/profile/games/cclcc/tipsmenu.cpp
        src/profile/games/cclcc/clearlistmenu.cpp
        src/profile/games/cclcc/tipsnotification.cpp
        src/profile/games/cclcc/mapsystem.cpp

        src/profile/games/mo6tw/dialoguebox.cpp
        src/profile/games/mo6tw/sysmesbox.cpp
        src/profile/games/mo6tw/systemmenu.cpp
        src/profile/games/mo6tw/titlemenu.cpp
        src/profile/games/mo6tw/savemenu.cpp
        src/profile/games/mo6tw/optionsmenu.cpp
        src/profile/games/mo6tw/tipsmenu.cpp
        src/profile/games/mo6tw/clearlistmenu.cpp
        src/profile/games/mo6tw/moviemenu.cpp
        src/profile/games/mo6tw/actorsvoicemenu.cpp
        src/profile/games/mo6tw/musicmenu.cpp
        src/profile/games/mo6tw/albummenu.cpp
        src/profile/games/mo6tw/tipsnotification.cpp

        src/profile/games/mo8/titlemenu.cpp

        src/profile/games/darling/sysmesbox.cpp

        src/games/rne/tilebackground.cpp
        src/games/rne/datedisplay.cpp
        src/games/rne/systemmenu.cpp
        src/games/rne/titlemenu.cpp
        src/games/rne/sysmesbox.cpp

        src/games/dash/titlemenu.cpp
        
        src/games/chlcc/dialoguebox.cpp
        src/games/chlcc/titlemenu.cpp
        src/games/chlcc/savemenu.cpp
        src/games/chlcc/sysmesbox.cpp
        src/games/chlcc/savesystem.cpp
        src/games/chlcc/tipssystem.cpp
        src/games/chlcc/clearlistmenu.cpp
        src/games/chlcc/moviemenu.cpp
        src/games/chlcc/albummenu.cpp
        src/games/chlcc/musicmenu.cpp
        src/games/chlcc/tipsmenu.cpp
        src/games/chlcc/optionsmenu.cpp
        src/games/chlcc/systemmenu.cpp
        src/games/chlcc/trophymenu.cpp
        src/games/chlcc/delusiontrigger.cpp

        src/games/cc/dialoguebox.cpp
        src/games/cc/titlemenu.cpp
        src/games/cc/sysmesbox.cpp

        src/games/cclcc/titlemenu.cpp
        src/games/cclcc/savemenu.cpp
        src/games/cclcc/optionsmenu.cpp
        src/games/cclcc/tipsmenu.cpp
        src/games/cclcc/clearlistmenu.cpp
        src/games/cclcc/tipsnotification.cpp
        src/games/cclcc/mapsystem.cpp

        src/games/mo6tw/dialoguebox.cpp
        src/games/mo6tw/sysmesbox.cpp
        src/games/mo6tw/systemmenu.cpp
        src/games/mo6tw/titlemenu.cpp
        src/games/mo6tw/savemenu.cpp
        src/games/mo6tw/optionsmenu.cpp
        src/games/mo6tw/tipsmenu.cpp
        src/games/mo6tw/savesystem.cpp
        src/games/mo6tw/tipssystem.cpp
        src/games/mo6tw/clearlistmenu.cpp
        src/games/mo6tw/moviemenu.cpp
        src/games/mo6tw/actorsvoicemenu.cpp
        src/games/mo6tw/musicmenu.cpp
        src/games/mo6tw/albummenu.cpp
        src/games/mo6tw/tipsnotification.cpp

        src/games/mo8/titlemenu.cpp

        src/games/darling/sysmesbox.cpp

        src/hud/dialoguebox.cpp
        src/hud/datedisplay.cpp
        src/hud/saveicondisplay.cpp
        src/hud/loadingdisplay.cpp
        src/hud/waiticondisplay.cpp
        src/hud/tipsnotification.cpp
        src/hud/delusiontrigger.cpp

        src/ui/ui.cpp
        src/ui/mapsystem.cpp
        src/ui/widget.cpp
        src/ui/menu.cpp
        src/ui/nullmenu.cpp
        src/ui/selectionmenu.cpp
        src/ui/sysmesbox.cpp
        src/ui/backlogmenu.cpp
        src/ui/tipsmenu.cpp
        src/ui/widgets/label.cpp
        src/ui/widgets/button.cpp
        src/ui/widgets/backlogentry.cpp
        src/ui/widgets/scrollbar.cpp
        src/ui/widgets/toggle.cpp
        src/ui/widgets/group.cpp
        src/ui/widgets/carousel.cpp
        src/ui/widgets/cgviewer.cpp
        src/ui/widgets/mo6tw/titlebutton.cpp
        src/ui/widgets/mo6tw/saveentrybutton.cpp
        src/ui/widgets/mo6tw/tipsentrybutton.cpp
        src/ui/widgets/mo6tw/scenelistentry.cpp
        src/ui/widgets/mo6tw/imagethumbnailbutton.cpp
        src/ui/widgets/mo6tw/albumthumbnailbutton.cpp
        src/ui/widgets/mo6tw/actorsvoicebutton.cpp
        src/ui/widgets/mo6tw/albumcharacterbutton.cpp
        src/ui/widgets/chlcc/titlebutton.cpp
        src/ui/widgets/chlcc/saveentrybutton.cpp
        src/ui/widgets/chlcc/systemmenuentrybutton.cpp
        src/ui/widgets/chlcc/moviemenuentrybutton.cpp
        src/ui/widgets/chlcc/albumthumbnailbutton.cpp
        src/ui/widgets/chlcc/trackselectbutton.cpp
        src/ui/widgets/rne/sysmenubutton.cpp
        src/ui/widgets/cc/titlebutton.cpp
        src/ui/widgets/cclcc/titlebutton.cpp

        src/stbi_impl.c

        src/renderer/3d/camera.cpp
        src/renderer/3d/model.cpp
        src/renderer/3d/transform.cpp
        src/renderer/3d/animation.cpp
        src/renderer/3d/modelanimator.cpp

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
        src/texture/webpdecode.cpp

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
        src/audio/audiostream.cpp
        src/audio/vorbisaudiostream.cpp
        src/audio/atrac9audiostream.cpp
        src/audio/adxaudiostream.cpp
        src/audio/hcaaudiostream.cpp

        src/video/videosystem.cpp
        src/video/videoplayer.cpp

        vendor/clHCA/clHCA.c

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
        src/util.h
        src/workqueue.h
        src/game.h
        src/mem.h
        src/modelviewer.h
        src/characterviewer.h
        src/spritesheet.h
        src/spriteanimation.h
        src/font.h
        src/background2d.h
        src/mask2d.h
        src/character2d.h
        src/text.h
        src/loadable.h
        src/inputsystem.h
        src/rng.h
        src/animation.h
        src/minilua_impl.h

        src/renderer/renderer.h
        src/renderer/window.h
        src/renderer/yuvframe.h

        src/data/savesystem.h
        src/data/tipssystem.h

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

        src/profile/data/savesystem.h
        src/profile/data/tipssystem.h

        src/profile/hud/saveicon.h
        src/profile/hud/loadingdisplay.h
        src/profile/hud/datedisplay.h
        src/profile/hud/tipsnotification.h
        src/profile/hud/delusiontrigger.h

        src/profile/ui/systemmenu.h
        src/profile/ui/titlemenu.h
        src/profile/ui/savemenu.h
        src/profile/ui/sysmesbox.h
        src/profile/ui/selectionmenu.h
        src/profile/ui/backlogmenu.h
        src/profile/ui/optionsmenu.h
        src/profile/ui/tipsmenu.h
        src/profile/ui/extramenus.h
        src/profile/ui/trophymenu.h
        src/profile/ui/mapsystem.h

        src/profile/games/rne/tilebackground.h
        src/profile/games/rne/systemmenu.h
        src/profile/games/rne/titlemenu.h
        src/profile/games/rne/sysmesbox.h

        src/profile/games/dash/titlemenu.h
        
        src/profile/games/chlcc/dialoguebox.h
        src/profile/games/chlcc/titlemenu.h
        src/profile/games/chlcc/savemenu.h
        src/profile/games/chlcc/sysmesbox.h
        src/profile/games/chlcc/clearlistmenu.h
        src/profile/games/chlcc/moviemenu.h
        src/profile/games/chlcc/albummenu.h
        src/profile/games/chlcc/musicmenu.h
        src/profile/games/chlcc/tipsmenu.h
        src/profile/games/chlcc/optionsmenu.h
        src/profile/games/chlcc/systemmenu.h
        src/profile/games/chlcc/trophymenu.h
        src/profile/games/chlcc/delusiontrigger.h
        
        src/profile/games/cc/dialoguebox.h
        src/profile/games/cc/titlemenu.h
        src/profile/games/cc/sysmesbox.h
        
        src/profile/games/cclcc/titlemenu.h
        src/profile/games/cclcc/savemenu.h
        src/profile/games/cclcc/optionsmenu.h
        src/profile/games/cclcc/tipsmenu.h
        src/profile/games/cclcc/clearlistmenu.h
        src/profile/games/cclcc/tipsnotification.h
        src/profile/games/cclcc/mapsystem.h

        src/profile/games/mo6tw/dialoguebox.h
        src/profile/games/mo6tw/sysmesbox.h
        src/profile/games/mo6tw/systemmenu.h
        src/profile/games/mo6tw/titlemenu.h
        src/profile/games/mo6tw/savemenu.h
        src/profile/games/mo6tw/optionsmenu.h
        src/profile/games/mo6tw/tipsmenu.h
        src/profile/games/mo6tw/clearlistmenu.h
        src/profile/games/mo6tw/moviemenu.h
        src/profile/games/mo6tw/actorsvoicemenu.h
        src/profile/games/mo6tw/musicmenu.h
        src/profile/games/mo6tw/albummenu.h
        src/profile/games/mo6tw/tipsnotification.h

        src/profile/games/mo8/titlemenu.h

        src/profile/games/darling/sysmesbox.h

        src/games/rne/tilebackground.h
        src/games/rne/datedisplay.h
        src/games/rne/systemmenu.h
        src/games/rne/titlemenu.h
        src/games/rne/sysmesbox.h

        src/games/dash/titlemenu.h
        
        src/games/chlcc/dialoguebox.h
        src/games/chlcc/titlemenu.h
        src/games/chlcc/savemenu.h
        src/games/chlcc/sysmesbox.h
        src/games/chlcc/savesystem.h
        src/games/chlcc/tipssystem.h
        src/games/chlcc/clearlistmenu.h
        src/games/chlcc/moviemenu.h
        src/games/chlcc/albummenu.h
        src/games/chlcc/musicmenu.h
        src/games/chlcc/tipsmenu.h
        src/games/chlcc/optionsmenu.h
        src/games/chlcc/systemmenu.h
        src/games/chlcc/trophymenu.h
        src/games/chlcc/delusiontrigger.h

        src/games/cc/dialoguebox.h
        src/games/cc/titlemenu.h
        src/games/cc/sysmesbox.h

        src/games/cclcc/titlemenu.h
        src/games/cclcc/savemenu.h
        src/games/cclcc/optionsmenu.h
        src/games/cclcc/tipsmenu.h
        src/games/cclcc/clearlistmenu.h
        src/games/cclcc/mapsystem.h

        src/games/mo6tw/dialoguebox.h
        src/games/mo6tw/sysmesbox.h
        src/games/mo6tw/systemmenu.h
        src/games/mo6tw/titlemenu.h
        src/games/mo6tw/savemenu.h
        src/games/mo6tw/optionsmenu.h
        src/games/mo6tw/tipsmenu.h
        src/games/mo6tw/savesystem.h
        src/games/mo6tw/tipssystem.h
        src/games/mo6tw/clearlistmenu.h
        src/games/mo6tw/moviemenu.h
        src/games/mo6tw/actorsvoicemenu.h
        src/games/mo6tw/musicmenu.h
        src/games/mo6tw/albummenu.h
        src/games/mo6tw/tipsnotification.h

        src/games/mo8/titlemenu.h

        src/games/darling/sysmesbox.h
        
        src/hud/dialoguebox.h
        src/hud/datedisplay.h
        src/hud/saveicondisplay.h
        src/hud/loadingdisplay.h
        src/hud/waiticondisplay.h
        src/hud/tipsnotification.h
        src/hud/delusiontrigger.h
        
        src/ui/ui.h
        src/ui/mapsystem.h
        src/ui/menu.h
        src/ui/nullmenu.h
        src/ui/selectionmenu.h
        src/ui/sysmesbox.h
        src/ui/backlogmenu.h
        src/ui/tipsmenu.h
        src/ui/widget.h
        src/ui/widgets/label.h
        src/ui/widgets/button.h
        src/ui/widgets/backlogentry.h
        src/ui/widgets/scrollbar.h
        src/ui/widgets/toggle.h
        src/ui/widgets/group.h
        src/ui/widgets/carousel.h
        src/ui/widgets/cgviewer.h
        src/ui/widgets/mo6tw/titlebutton.h
        src/ui/widgets/mo6tw/saveentrybutton.h
        src/ui/widgets/mo6tw/tipsentrybutton.h
        src/ui/widgets/mo6tw/scenelistentry.h
        src/ui/widgets/mo6tw/imagethumbnailbutton.h
        src/ui/widgets/mo6tw/albumthumbnailbutton.h
        src/ui/widgets/mo6tw/actorsvoicebutton.h
        src/ui/widgets/mo6tw/albumcharacterbutton.h
        src/ui/widgets/chlcc/titlebutton.h
        src/ui/widgets/chlcc/saveentrybutton.h
        src/ui/widgets/chlcc/systemmenuentrybutton.h
        src/ui/widgets/chlcc/moviemenuentrybutton.h
        src/ui/widgets/chlcc/albumthumbnailbutton.h
        src/ui/widgets/chlcc/trackselectbutton.h
        src/ui/widgets/rne/sysmenubutton.h
        src/ui/widgets/cc/titlebutton.h
        src/ui/widgets/cclcc/titlebutton.h
        
        src/renderer/3d/camera.h
        src/renderer/3d/model.h
        src/renderer/3d/renderable3d.h
        src/renderer/3d/transform.h
        src/renderer/3d/scene.h
        src/renderer/3d/animation.h
        src/renderer/3d/modelanimator.h

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

        src/audio/audiobackend.h
        src/audio/audiosystem.h
        src/audio/audiocommon.h
        src/audio/audiochannel.h
        src/audio/audiostream.h
        src/audio/buffering.h
        src/audio/ffmpegaudioplayer.h
        src/audio/vorbisaudiostream.h
        src/audio/atrac9audiostream.h
        src/audio/adxaudiostream.h
        src/audio/hcaaudiostream.h

        src/video/videosystem.h
        src/video/videoplayer.h

        vendor/clHCA/clHCA.h

        vendor/squish/squish.h

        vendor/utf8-cpp/utf8.h

        vendor/vma/vk_mem_alloc.h

        vendor/minilua/minilua.h
)

if (WIN32)
    list(APPEND Impacto_Src
            src/manifest_windows.manifest
    )
endif ()

if (NX OR EMSCRIPTEN)
    set(IMPACTO_DISABLE_VULKAN ON)
    set(IMPACTO_DISABLE_DX9 ON)
endif ()

if (APPLE OR LINUX)
    set(IMPACTO_DISABLE_DX9 ON)
endif ()

# dependencies

if (WIN32)
    # Workaround for RelWithDebInfo builds not installing all the libraries
    set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
    set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

    set(SDL2_LIBRARIES SDL2::SDL2 SDL2::SDL2main)
    set(LIBATRAC9_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/LibAtrac9/include")

    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(LIBATRAC9_ARCH "x64")
    else ()
        set(LIBATRAC9_ARCH "x86")
    endif ()

    set(LIBATRAC9_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/LibAtrac9/bin/${LIBATRAC9_ARCH}/libatrac9.lib")
endif ()

find_package(Libatrac9 REQUIRED)
find_package(glm REQUIRED)

if (NX) # avoid CMAKE_DL_LIBS for NX
    set(Impacto_Libs
            ${LIBATRAC9_LIBRARY}
    )
else ()
    set(Impacto_Libs
            ${CMAKE_DL_LIBS}
            ${LIBATRAC9_LIBRARY}
    )
endif ()

if (NOT DEFINED IMPACTO_DISABLE_OPENAL)
    find_package(OpenAL REQUIRED)

    list(APPEND Impacto_Libs
            ${OPENAL_LIBRARY}
    )

    list(APPEND Impacto_Src
            src/audio/openal/audiobackend.cpp
            src/audio/openal/audiochannel.cpp
            src/audio/openal/ffmpegaudioplayer.cpp
    )
    list(APPEND Impacto_Header
            src/audio/openal/audiobackend.h
            src/audio/openal/audiochannel.h
            src/audio/openal/audiocommon.h
            src/audio/openal/ffmpegaudioplayer.h
    )

endif ()

if (EMSCRIPTEN)
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
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(IMPACTO_EMSCRIPTEN_BUILD_FLAGS "${IMPACTO_EMSCRIPTEN_BUILD_FLAGS} \
            -s GL_ASSERTIONS=1 \
            -s GL_DEBUG=1 \
        ")
    else ()
        set(IMPACTO_EMSCRIPTEN_BUILD_FLAGS "${IMPACTO_EMSCRIPTEN_BUILD_FLAGS} \
            -O3 \
        ")
    endif ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${IMPACTO_EMSCRIPTEN_BUILD_FLAGS}")

    string(REPLACE "-O2" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    string(REPLACE "-O2" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
    string(REPLACE "-O2" "" CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

    add_definitions(-DIMPACTO_OPENAL_INCLUDE_SUBDIR)
else ()
    find_package(SDL2 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(OggVorbis REQUIRED)
    find_package(WebP REQUIRED)

    include_directories(${OGGVORBIS_INCLUDE_DIRS})

    if (NX)
        list(APPEND Impacto_Libs
                ${SDL2_LIBRARIES}
                glm
                ZLIB::ZLIB
                vorbisfile
                vorbis
                ogg
                webp
                EGL
                glapi
                drm_nouveau
                nx
        )
    
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group") # workaround for SDL2 being last in the linker list
    
        add_definitions(-DIMPACTO_OPENAL_INCLUDE_SUBDIR)
    else ()
        # Add this because apparently find_package(SDL2 REQUIRED) doesn't work right on Arch?
        if ("${SDL2_LIBRARIES}" STREQUAL "")
            set(SDL2_LIBRARIES "SDL2::SDL2")
        endif ()
    
        list(APPEND Impacto_Libs
                ${SDL2_LIBRARIES}
                ZLIB::ZLIB
                ${OGGVORBIS_LIBRARIES}
                ${WebP_LIBRARIES}
        )
    
        if (VCPKG_TOOLCHAIN)
            list(APPEND Impacto_Libs
                    glm::glm
            )
        elseif (NOT APPLE) # If we don't do this it decides to link glm as a library on macOS for some reason
            list(APPEND Impacto_Libs
                    glm
            )
        endif ()
    
        add_definitions(-DIMPACTO_OPENAL_HAVE_ALEXT)
    endif ()
endif ()

include_directories(${LIBATRAC9_INCLUDE_DIR})
if (NOT DEFINED IMPACTO_DISABLE_OPENAL)
    include_directories(${OPENAL_INCLUDE_DIR})
endif ()
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${WebP_INCLUDE_DIRS})
include_directories(vendor/include)
include_directories(vendor/pcg/include)

if (NOT DEFINED IMPACTO_DISABLE_OPENGL)
    list(APPEND Impacto_Src
            src/renderer/opengl/window.cpp
            src/renderer/opengl/renderer.cpp
            src/renderer/opengl/shader.cpp
            src/renderer/opengl/glc.cpp
            src/renderer/opengl/yuvframe.cpp
            src/renderer/opengl/3d/renderable3d.cpp
            src/renderer/opengl/3d/scene.cpp

            src/nuklear_impl.cpp

            vendor/glad/src/glad.c
    )
    list(APPEND Impacto_Header
            src/renderer/opengl/window.h
            src/renderer/opengl/renderer.h
            src/renderer/opengl/shader.h
            src/renderer/opengl/glc.h
            src/renderer/opengl/yuvframe.h
            src/renderer/opengl/3d/renderable3d.h
            src/renderer/opengl/3d/scene.h

            vendor/nuklear/nuklear.h
            vendor/nuklear/nuklear_sdl_gl3.h
    )

    include_directories(vendor/glad/include)
endif ()

if (NOT DEFINED IMPACTO_DISABLE_FFMPEG)
    list(APPEND Impacto_Src
            src/video/ffmpegplayer.cpp
            src/video/ffmpegstream.cpp
    )
    list(APPEND Impacto_Header
            src/video/ffmpegplayer.h
            src/video/ffmpegstream.h
    )

    find_package(FFMPEG REQUIRED)

    list(APPEND Impacto_Libs
            ${FFMPEG_LIBRARIES}
    )
    include_directories(${FFMPEG_INCLUDE_DIRS})
endif ()

if (NOT DEFINED IMPACTO_DISABLE_MSPACK)
    list(APPEND Impacto_Src
            src/io/lzxstream.cpp

            vendor/mspack/lzxd.c
            vendor/mspack/system.c
    )
    list(APPEND Impacto_Header
            src/io/lzxstream.h
    )
    include_directories(vendor/mspack)
endif ()

if (NOT DEFINED IMPACTO_DISABLE_VULKAN)
    list(APPEND Impacto_Src
            src/renderer/vulkan/window.cpp
            src/renderer/vulkan/renderer.cpp
            src/renderer/vulkan/pipeline.cpp
            src/renderer/vulkan/utils.cpp
            src/renderer/vulkan/yuvframe.cpp
            src/renderer/vulkan/3d/renderable3d.cpp
            src/renderer/vulkan/3d/scene.cpp
    )
    list(APPEND Impacto_Header
            src/renderer/vulkan/window.h
            src/renderer/vulkan/renderer.h
            src/renderer/vulkan/pipeline.h
            src/renderer/vulkan/utils.h
            src/renderer/vulkan/yuvframe.h
            src/renderer/vulkan/3d/renderable3d.h
            src/renderer/vulkan/3d/scene.h
    )

    find_package(Vulkan REQUIRED)

    list(APPEND Impacto_Libs
            ${Vulkan_LIBRARIES}
    )
    include_directories(${Vulkan_INCLUDE_DIRS})
endif ()

if (NOT DEFINED IMPACTO_DISABLE_DX9)
    list(APPEND Impacto_Src
            src/renderer/dx9/utils.cpp
            src/renderer/dx9/renderer.cpp
            src/renderer/dx9/window.cpp
            src/renderer/dx9/shader.cpp
            src/renderer/dx9/yuvframe.cpp
            src/renderer/dx9/3d/scene.cpp
            src/renderer/dx9/3d/renderable3d.cpp
    )
    list(APPEND Impacto_Header
            src/renderer/dx9/utils.h
            src/renderer/dx9/renderer.h
            src/renderer/dx9/window.h
            src/renderer/dx9/shader.h
            src/renderer/dx9/yuvframe.h
            src/renderer/dx9/3d/scene.h
            src/renderer/dx9/3d/renderable3d.h 
    )

    list(APPEND Impacto_Libs
            d3d9
            d3dcompiler
    )
endif ()

add_executable(impacto ${Impacto_Src} ${Impacto_Header})

target_link_libraries(impacto PUBLIC ${Impacto_Libs})

# compiler/dependency configuration

set_property(TARGET impacto PROPERTY CXX_STANDARD 14)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

if (MSVC)
    add_definitions(-DBETTER_ENUMS_NO_CONSTEXPR) # broken in MSVC 2017 apparently
endif ()

if (NX)
    set_property(TARGET impacto PROPERTY NO_SYSTEM_FROM_IMPORTED ON) # to avoid -isystem
    add_definitions(-DENV64BIT)
else ()
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DENV64BIT)
    else ()
        add_definitions(-DENV32BIT)
    endif ()
endif ()

add_definitions(-DGLM_FORCE_RADIANS) # lol

# target configuration

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT ON)
    set(IMPACTO_GL_DEBUG_DEFAULT ON)
else ()
    set(IMPACTO_ENABLE_SLOW_LOG_DEFAULT OFF)
    set(IMPACTO_GL_DEBUG_DEFAULT OFF)
endif ()

option(IMPACTO_ENABLE_SLOW_LOG
        "Compile log statements that get hit very frequently or are in a hot path"
        ${IMPACTO_ENABLE_SLOW_LOG_DEFAULT})
option(IMPACTO_GL_DEBUG
        "Use an OpenGL debug context and log messages"
        ${IMPACTO_GL_DEBUG_DEFAULT})

if (EMSCRIPTEN)
    set(IMPACTO_HAVE_THREADS OFF)
    set(IMPACTO_USE_SDL_HIGHDPI ON)
else ()
    set(IMPACTO_HAVE_THREADS ON)
    set(IMPACTO_USE_SDL_HIGHDPI OFF)
endif ()

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
endif ()

# asset install

install(DIRECTORY src/shaders DESTINATION .)
install(DIRECTORY profiles DESTINATION .)
install(DIRECTORY games DESTINATION .)