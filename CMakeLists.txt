cmake_minimum_required(VERSION 3.16)
project(HoustonV2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PCH_HEADER "${CMAKE_SOURCE_DIR}/src/pch.h")

add_executable(HoustonV2 src/main.cpp
        src/SerialHelper.h
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
)
target_precompile_headers(HoustonV2 PRIVATE ${PCH_HEADER})

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


find_package(Boost REQUIRED COMPONENTS system)



target_link_libraries(HoustonV2 PRIVATE Boost::boost Boost::system)
target_include_directories(HoustonV2 PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(HoustonV2 PRIVATE ${OPENGL_LIBRARIES})
