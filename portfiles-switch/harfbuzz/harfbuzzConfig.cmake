if(NOT TARGET harfbuzz::harfbuzz)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(_switch_harfbuzz REQUIRED IMPORTED_TARGET harfbuzz)

    add_library(harfbuzz::harfbuzz INTERFACE IMPORTED)
    set_property(TARGET harfbuzz::harfbuzz PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::_switch_harfbuzz)
endif()
