cmake_minimum_required(VERSION 3.31)
project(MiisendU-Wii-U)

find_package(PkgConfig REQUIRED)

include(FetchContent)
FetchContent_Declare(
  inih
  URL https://github.com/benhoyt/inih/archive/refs/tags/r62.tar.gz
)
FetchContent_Declare(
  rapidjson
  URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz
)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Disable rapidjson documentation build" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Disable rapidjson examples build" FORCE)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Disable rapidjson tests build" FORCE)
set(RAPIDJSON_BUILD_CXX11 OFF CACHE BOOL "Disable C++11" FORCE)
set(RAPIDJSON_BUILD_CXX17 OFF CACHE BOOL "Disable C++17" FORCE)
set(RAPIDJSON_BUILD_CXX20 ON CACHE BOOL "Enable C++20" FORCE)
FetchContent_MakeAvailable(inih rapidjson)

add_executable(MiisendU-Wii-U)

target_compile_features(MiisendU-Wii-U PRIVATE cxx_std_23)

target_compile_options(MiisendU-Wii-U PRIVATE
  -Werror
  -Wextra
  -Wshadow
  -Wpedantic
  -Wnull-dereference
  -Wmisleading-indentation
  -Wduplicated-cond
  -Wduplicated-branches
)

target_sources(MiisendU-Wii-U PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/console.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/udp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vpad_to_json.cpp
  ${inih_SOURCE_DIR}/ini.c
)

target_include_directories(MiisendU-Wii-U SYSTEM PRIVATE
  ${inih_SOURCE_DIR}
  ${rapidjson_SOURCE_DIR}/include
)

target_include_directories(MiisendU-Wii-U PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(MiisendU-Wii-U PRIVATE
)

wut_create_rpx(MiisendU-Wii-U)

wut_create_wuhb(MiisendU-Wii-U
  CONTENT
  NAME "MiisendU Wii U"
  SHORTNAME "MiisendU"
  AUTHOR "Crayon"
  ICON "wuhb/icon.png"
  TVSPLASH "wuhb/tv-image.png"
  DRCSPLASH "wuhb/drc-image.png"
)
