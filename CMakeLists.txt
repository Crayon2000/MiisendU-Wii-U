cmake_minimum_required(VERSION 3.18)
project(MiisendU-Wii-U)

find_package(PkgConfig REQUIRED)

include(FetchContent)
FetchContent_Declare(
  inih
  URL https://github.com/benhoyt/inih/archive/refs/tags/r58.tar.gz
)
FetchContent_Declare(
  rapidjson
  URL https://github.com/Tencent/rapidjson/archive/refs/heads/master.tar.gz
)
FetchContent_Populate(rapidjson)
FetchContent_MakeAvailable(inih)

add_executable(MiisendU-Wii-U)

target_compile_features(MiisendU-Wii-U PRIVATE cxx_std_20)

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
