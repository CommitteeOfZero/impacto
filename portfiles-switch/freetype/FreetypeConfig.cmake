if(NOT TARGET Freetype::Freetype)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(_switch_freetype REQUIRED IMPORTED_TARGET freetype2)

    add_library(Freetype::Freetype INTERFACE IMPORTED)
    set_property(TARGET Freetype::Freetype PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_freetype)
endif()
