cmake_minimum_required(VERSION 3.18)
project(MiisendU-Wii-U)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JANSSON REQUIRED jansson)

include(FetchContent)
FetchContent_Declare(
  inih
  URL https://github.com/benhoyt/inih/archive/refs/tags/r58.tar.gz
)
FetchContent_MakeAvailable(inih)

add_executable(MiisendU-Wii-U)

target_sources(MiisendU-Wii-U PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/source/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/console.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/udp.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vpad_to_json.cpp
  ${inih_SOURCE_DIR}/ini.c
)

target_include_directories(MiisendU-Wii-U SYSTEM PRIVATE
  ${JANSSON_INCLUDE_DIRS}
  ${inih_SOURCE_DIR}
)

target_include_directories(MiisendU-Wii-U PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(MiisendU-Wii-U PRIVATE
  ${JANSSON_LIBRARIES}
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
