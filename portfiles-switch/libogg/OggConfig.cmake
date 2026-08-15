if(NOT TARGET Ogg::ogg)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(_switch_ogg REQUIRED IMPORTED_TARGET ogg)

    add_library(Ogg::ogg INTERFACE IMPORTED)
    set_property(TARGET Ogg::ogg PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_ogg)
endif()
