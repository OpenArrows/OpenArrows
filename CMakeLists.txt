cmake_minimum_required(VERSION 3.10)
project(DemocracyArrows LANGUAGES C VERSION 0.1.0)

# Libraries

# Vulkan is required to pre-compile GLSL shaders to SPIR-V (glslc)
find_package(Vulkan REQUIRED COMPONENTS glslc)

add_subdirectory(lib/glfw)

add_subdirectory(lib/glad/cmake glad_cmake)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.3)

include_directories(build)

# Pre-compile shaders
set(GLSLC_FLAGS -DGLSL)

file(GLOB_RECURSE SHADERS RELATIVE "${CMAKE_SOURCE_DIR}/src/shaders/" src/shaders/*.frag src/shaders/*.vert src/shaders/*.comp)
foreach(SHADER IN LISTS SHADERS)
    # The resulting binary files will be later embedded into executable using C23's #embed directive
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${SHADER}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSLC_FLAGS} "${CMAKE_SOURCE_DIR}/src/shaders/${SHADER}" -o ${SPIRV}
        DEPENDS src/shaders/${SHADER}
        VERBATIM)
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(SHADER IN LISTS SHADERS)

add_custom_target(shaders ALL DEPENDS ${SPIRV_BINARY_FILES})

# Game executable
add_executable(game src/main.c)
target_link_libraries(game PUBLIC glfw glad ${SHADERS_BIN})

install(TARGETS game)