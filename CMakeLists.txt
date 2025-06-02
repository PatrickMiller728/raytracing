cmake_minimum_required(VERSION 3.31.6)

include(external/Walnut/vendor/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(raytracing)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/src/**.h
        ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/src/**.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

add_subdirectory(external/Walnut)

target_link_libraries(${PROJECT_NAME} walnut)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/RayTracing/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/Walnut/Walnut/Platform/GUI)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/Walnut/Walnut/Source)