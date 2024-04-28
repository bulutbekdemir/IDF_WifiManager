cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS components/) # Add the components directory to the project
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(IDF_WifiManager)
