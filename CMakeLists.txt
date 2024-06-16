cmake_minimum_required(VERSION 3.18)
project(TexViewer VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /O2 /W3 /std:c++20 /Zc:__cplusplus /permissive-")
elseif(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb")
endif()

configure_file(include/Meta.h.in include/Meta.h)

file(GLOB TEX_VIEWER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*
    ${CMAKE_CURRENT_SOURCE_DIR}/include/MainWindow.h
)
set(GLOB TEX_VIEWER_SOURCES_UI CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/MainWindow.ui
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/bcdec
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Binary-Reader/include
)

# Qt stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

add_executable(TexViewer ${TEX_VIEWER_SOURCES} ${TEX_VIEWER_SOURCES_UI})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
target_link_libraries(TexViewer spdlog Qt6::Core Qt6::Gui Qt6::Widgets)

install(
    TARGETS TexViewer
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)