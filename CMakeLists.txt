cmake_minimum_required(VERSION 3.16)
project(HoustonV2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(HoustonV2 main.cpp
        SerialHelper.h
        Setting.h
        Component.h
        ControlPanel.cpp
        ControlPanel.h
        TelemetryPanel.cpp
        TelemetryPanel.h
        Util.h
        AlertPanel.cpp
        AlertPanel.h
        LiveDataPanel.cpp
        LiveDataPanel.h)
find_package(glad CONFIG REQUIRED)
target_link_libraries(HoustonV2 PRIVATE glad::glad)
find_package(asio CONFIG REQUIRED)
target_link_libraries(HoustonV2 PRIVATE asio::asio)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(HoustonV2 PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(HoustonV2 PRIVATE imgui::imgui)
find_package(implot CONFIG REQUIRED)
target_link_libraries(HoustonV2 PRIVATE implot::implot)
find_package(OpenGL REQUIRED)
#target_link_libraries(HoustonV2 PRIVATE opengl32.lib)
target_link_libraries(HoustonV2 PRIVATE ${OPENGL_LIBRARIES})
