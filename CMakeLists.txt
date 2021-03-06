CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
CMAKE_POLICY(VERSION 2.6)
IF(LUAROCKS_PREFIX)
    MESSAGE(STATUS "Installing Torch through Luarocks")
    STRING(REGEX REPLACE "(.*)lib/luarocks/rocks.*" "\\1" CMAKE_INSTALL_PREFIX  "${LUAROCKS_PREFIX}")
    MESSAGE(STATUS "Prefix inferred from Luarocks: ${CMAKE_INSTALL_PREFIX}")
ENDIF()
FIND_PACKAGE(Torch REQUIRED)

if (UNIX AND NOT APPLE)
    add_subdirectory (linux)
endif (UNIX AND NOT APPLE)

if (APPLE)
    add_subdirectory (opencv)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_subdirectory (macos)
    endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
endif (APPLE)

if (NOT UNIX)
    message (ERROR "This package only builds on Unix platforms")
endif (NOT UNIX)

install_files(/lua/map init.lua)

SET(src)
SET(luasrc linux/init.lua)
ADD_TORCH_PACKAGE(map "${src}" "${luasrc}" "Image Processing")
