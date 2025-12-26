cmake_minimum_required(VERSION 3.16)
project(HoustonV2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PCH_HEADER "${CMAKE_SOURCE_DIR}/src/pch.h")
include(FetchContent)

add_executable(HoustonV2 src/main.cpp
        src/producer/SerialProducer.h
        src/Setting.h
        src/components/Component.h
        src/components/ControlPanel.cpp
        src/components/ControlPanel.h
        src/components/TelemetryPanel.cpp
        src/components/TelemetryPanel.h
        src/Util.h
        src/components/AlertPanel.cpp
        src/components/AlertPanel.h
        src/components/LiveDataPanel.cpp
        src/components/LiveDataPanel.h
        src/consumer/DataConsumer.h
        src/consumer/QueueData.h
        src/consumer/QueueData.cpp
        src/Dispatcher.h
        src/producer/DataProducer.h
        src/dashboard/Dashboard.c
        src/dashboard/COBS.c
        src/consumer/TypedConsumer.h
        src/consumer/AlertConsumer.h
        src/consumer/TelemetryConsumer.h
)
target_precompile_headers(HoustonV2 PRIVATE ${PCH_HEADER})

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking  # <--- The Magic Word
)

# Creates ${imgui_SOURCE_DIR} variable and downloads the content into it
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        # ----------------------------------------------------------
        # PLATFORM BACKENDS (Change these if using SDL/Vulkan/DX11)
        # ----------------------------------------------------------
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}           # For "imgui.h"
        ${imgui_SOURCE_DIR}/backends  # For "imgui_impl_glfw.h"
)

# GLFW Options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
if (WIN32)
    message(STATUS "Configuring for Windows...")
    set(GLFW_BUILD_WIN32 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
elseif (APPLE)
    message(STATUS "Configuring for macOS...")
    set(GLFW_BUILD_COCOA ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
elseif (UNIX)
    message(STATUS "Configuring for Linux (X11)...")
    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif ()
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# Compile Implot ourself instead of using FetchContent_MakeAvailable
FetchContent_Declare(
        implot
        GIT_REPOSITORY https://github.com/epezent/implot
        GIT_TAG master
)
FetchContent_GetProperties(implot)
if (NOT implot_POPULATED)
    FetchContent_Populate(implot)
endif ()
add_library(implot STATIC
        ${implot_SOURCE_DIR}/implot.cpp
        ${implot_SOURCE_DIR}/implot_items.cpp
        ${implot_SOURCE_DIR}/implot_demo.cpp
)
target_include_directories(implot PUBLIC ${implot_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)

find_package(OpenGL REQUIRED)

#linux requires sudo apt-get install libx11-dev
if (UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(imgui PUBLIC ${X11_LIBRARIES})
endif ()

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_TAG asio-1-36-0
)
FetchContent_GetProperties(asio)
if (NOT asio_POPULATED)
    FetchContent_Populate(asio)
endif ()
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

if (WIN32)
    target_link_libraries(asio INTERFACE ws2_32)
elseif(UNIX AND NOT APPLE)
    # Linux (X11) specific linking
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)

    # ImGui Docking backend touches X11 directly
    target_link_libraries(imgui PUBLIC ${X11_LIBRARIES})

    # Asio needs threads/pthread on Linux
    target_link_libraries(asio INTERFACE Threads::Threads)
    target_link_libraries(asio INTERFACE dl)
endif()
target_link_libraries(HoustonV2 PRIVATE imgui)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
target_link_libraries(HoustonV2 PRIVATE implot)
target_link_libraries(HoustonV2 PRIVATE asio)
