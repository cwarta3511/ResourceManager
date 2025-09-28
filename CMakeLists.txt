cmake_minimum_required(VERSION 3.10)
project(ResourceManager VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SDL2 (adjust if you installed it differently)
find_package(SDL2 REQUIRED)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
)

add_executable(ResourceManager main.cpp ${IMGUI_SOURCES})

target_include_directories(ResourceManager PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(ResourceManager PRIVATE
    ${SDL2_LIBRARIES}
    "-framework OpenGL"
)
