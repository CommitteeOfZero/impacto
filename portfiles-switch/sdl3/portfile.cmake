vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO devkitPro/SDL
    REF c329016c0e338ab4397ad930f83330c9fe058348
    SHA512 2f261c9f3715f933180d905f94a5f8bf1cb6bf0ac7975704cd1e6cbaddd1a9da24edf25caab4891e55e23141bb156495d6e0212ebaa2741577079981be23dd3d
    HEAD_REF switch-sdl-3.4
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DNINTENDO_SWITCH=ON
        -DSDL_STATIC=ON
        -DSDL_SHARED=OFF
        -DSDL_TEST_LIBRARY=OFF
        -DSDL_TESTS=OFF
        -DSDL_INSTALL_CMAKEDIR_ROOT=share/${PORT}
        # Specifying the revision skips the need to use git to determine a version
        -DSDL_REVISION=vcpkg
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup()

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)

vcpkg_copy_pdbs()
vcpkg_fixup_pkgconfig()

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.txt"
    COMMENT "Built from devkitPro/SDL@switch-sdl-3.4, some configurations may use code licensed under the MIT and Apache-2.0 licenses."
)
