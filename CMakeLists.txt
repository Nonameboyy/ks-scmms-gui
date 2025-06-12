cmake_minimum_required(VERSION 3.6)

project(ks-scmms-gui VERSION 2.3)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TARGET_NAME ks-scmms-gui)

find_package(PkgConfig REQUIRED)
find_package(
    Qt5
    COMPONENTS Core Gui Widgets WebEngineWidgets
    REQUIRED)

add_subdirectory(src)
add_subdirectory(data)
