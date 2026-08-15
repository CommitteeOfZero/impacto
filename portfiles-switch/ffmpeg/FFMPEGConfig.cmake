find_package(PkgConfig REQUIRED)

if(NOT TARGET FFMPEG)
    pkg_check_modules(_switch_ffmpeg REQUIRED IMPORTED_TARGET 
        libavcodec 
        libavutil 
        libswscale
        libswresample
        libavformat 
    )
    add_library(FFMPEG INTERFACE IMPORTED)
    set_property(TARGET FFMPEG PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_ffmpeg)
endif()