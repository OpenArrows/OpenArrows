cmake_minimum_required(VERSION 3.10)
project(DemocracyArrows LANGUAGES C VERSION 0.1.0)

# Libraries

# Vulkan is required to pre-compile GLSL shaders to SPIR-V (glslc)
find_package(Vulkan REQUIRED COMPONENTS glslc)

add_subdirectory(lib/glfw)

add_subdirectory(lib/glad/cmake glad_cmake)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=3.3)

add_library(whereami STATIC lib/whereami/src/whereami.c)
include_directories(lib/whereami/src)

# Game executable
add_executable(game src/main.c)
target_link_libraries(game PUBLIC glfw glad whereami)

# Pre-compile shaders
file(GLOB_RECURSE SHADERS RELATIVE "${CMAKE_SOURCE_DIR}/src/shaders/" src/shaders/*.frag src/shaders/*.vert src/shaders/*.comp)
foreach(SHADER IN LISTS SHADERS)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${SHADER}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} "${CMAKE_SOURCE_DIR}/src/shaders/${SHADER}" -o ${SPIRV}
        DEPENDS src/shaders/${SHADER}
        VERBATIM)
    list(APPEND SPIRV_OUTPUT ${SPIRV})
    list(APPEND SPIRV_BINARY_FILES ${SHADER}.spv)
endforeach(SHADER IN LISTS SHADERS)

set(SHADERS_BIN "${PROJECT_BINARY_DIR}/shaders.o")
add_custom_command(
    OUTPUT ${SHADERS_BIN}
    WORKING_DIRECTORY "${PROJECT_BINARY_DIR}/shaders"
    COMMAND ld -r -b binary -o ../shaders.o ${SPIRV_BINARY_FILES}
    DEPENDS ${SPIRV_OUTPUT})

add_custom_target(
    shaders 
    DEPENDS ${SHADERS_BIN})

add_dependencies(game shaders)

install(TARGETS game)