find_package(PkgConfig REQUIRED)

if(NOT TARGET Vorbis::vorbis)
    pkg_check_modules(_switch_vorbis REQUIRED IMPORTED_TARGET vorbis)
    add_library(Vorbis::vorbis INTERFACE IMPORTED)
    set_property(TARGET Vorbis::vorbis PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_vorbis)
endif()

if(NOT TARGET Vorbis::vorbisfile)
    pkg_check_modules(_switch_vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
    add_library(Vorbis::vorbisfile INTERFACE IMPORTED)
    set_property(TARGET Vorbis::vorbisfile PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_vorbisfile)
endif()
