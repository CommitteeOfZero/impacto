cmake_minimum_required(VERSION 3.20)

project(atrac9 LANGUAGES C)

include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR_MESSAGE)
option(LIBATRAC9_BUILD_SHARED "Builds libatrac9 as a shared library" ON)

set(LIBATRAC9_SOURCES 
    C/src/band_extension.c
    C/src/bit_allocation.c
    C/src/bit_reader.c
    C/src/decinit.c
    C/src/decoder.c
    C/src/huffCodes.c
    C/src/imdct.c
    C/src/libatrac9.c
    C/src/quantization.c
    C/src/scale_factors.c
    C/src/tables.c
    C/src/unpack.c
    C/src/utility.c
)

if (LIBATRAC9_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED
        ${LIBATRAC9_SOURCES})
else()
    add_library(${PROJECT_NAME} STATIC
        ${LIBATRAC9_SOURCES})
endif()

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE COMPILING_DLL)
    set(LIBATRAC9_BUILD_SHARED ON)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_library(atrac9::atrac9 ALIAS ${PROJECT_NAME})

file(COPY C/src/libatrac9.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libatrac9)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/libatrac9/libatrac9.h)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC include)


if(IPO_SUPPORTED)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${IPO_ERROR_MESSAGE}")
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libatrac9
)
