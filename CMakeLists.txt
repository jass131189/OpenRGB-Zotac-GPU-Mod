cmake_minimum_required(VERSION 3.16)
project(OpenRGBZotac5070TiPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# OpenRGB 1.0 / 1.1 uses Qt 6; OpenRGB 0.9 uses Qt 5
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(QT_WIDGETS_TARGET Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_WIDGETS_TARGET Qt5::Widgets)
endif()

# Point OPENRGB_SRC_DIR to your local clone of https://gitlab.com/CalcProgrammer1/OpenRGB
set(OPENRGB_SRC_DIR "${CMAKE_SOURCE_DIR}/../OpenRGB" CACHE PATH "Path to OpenRGB source tree")

add_library(OpenRGBZotac5070TiPlugin SHARED
    OpenRGBZotac5070TiPlugin.h
    OpenRGBZotac5070TiPlugin.cpp
    RGBController_Zotac5070Ti.h
    RGBController_Zotac5070Ti.cpp
    ${OPENRGB_SRC_DIR}/RGBController/RGBController.cpp
    ${OPENRGB_SRC_DIR}/i2c_smbus/i2c_smbus.cpp
)

target_include_directories(OpenRGBZotac5070TiPlugin PRIVATE
    ${OPENRGB_SRC_DIR}
    ${OPENRGB_SRC_DIR}/OpenRGBPluginInterface
    ${OPENRGB_SRC_DIR}/RGBController
    ${OPENRGB_SRC_DIR}/i2c_smbus
    ${OPENRGB_SRC_DIR}/ResourceManager
)

target_link_libraries(OpenRGBZotac5070TiPlugin PRIVATE ${QT_WIDGETS_TARGET})
