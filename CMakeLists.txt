cmake_minimum_required(VERSION 3.20)
project(CimaEngine3v LANGUAGES CXX)

set(BIN "${CMAKE_SOURCE_DIR}/bin")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN}/${CMAKE_BUILD_TYPE}")

add_compile_definitions(ASSETS="${CMAKE_SOURCE_DIR}/assets")
#bandera para Debug O RELEASE
add_compile_definitions("DEBUG=$<CONFIG:Debug>")

#Descargar las dependencias (IMGUI y SFML)
message("Descargando SFML")
include(FetchContent)
if(WIN32)
    FetchContent_Declare(
        SFML
        URL "https://github.com/SFML/SFML/archive/refs/heads/3.0.x.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)
else()
    find_package(SFML 3 COMPONENTS System Window Graphics Audio REQUIRED)
endif()
message("Descargando IMGUI")

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.1-docking
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(imgui)

message("Descargando ImguiSFML")

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)

FetchContent_MakeAvailable(imgui-sfml)
#incluir los headers
include_directories(${SFML_INCLUDE_DIRS})
include_directories(${IMGUI_DIR})
#include_directories(${S_INCLUDE_DIRS})

#incluir los folders
add_subdirectory(src)
#ligar las librerias
target_link_libraries(
    ${PROJECT_NAME}
    SFML::System SFML::Window SFML::Graphics SFML::Audio
    ImGui-SFML::ImGui-SFML
)
#asignar el estandar
target_compile_features(CimaEngine3v PUBLIC cxx_std_17)