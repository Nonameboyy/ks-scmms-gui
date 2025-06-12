cmake_minimum_required(VERSION 2.8)

FILE(GLOB KS_SCMMS_GUI_SRC_LIST_H "./*.h")
FILE(GLOB KS_SCMMS_GUI_SRC_LIST_CPP "./*.cpp")
FILE(GLOB KS_SCMMS_GUI_SRC_LIST_UI "./*.ui")
FILE(GLOB_RECURSE KS_SCMMS_GUI_SRC_LIST_RESOURCES "../data/*.qrc")

ADD_EXECUTABLE(${TARGET_NAME}
    ${KS_SCMMS_GUI_SRC_LIST_CPP}
    ${KS_SCMMS_GUI_SRC_LIST_H}
    ${KS_SCMMS_GUI_SRC_LIST_UI}
    ${KS_SCMMS_GUI_SRC_LIST_RESOURCES})

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${TARGET_NAME}
    Qt5::Gui
    Qt5::Widgets
    Qt5::Core
    Qt5::WebEngineWidgets
    -L/usr/lib64/libvpx.so.1
    -lGL)


install(TARGETS ${TARGET_NAME} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
