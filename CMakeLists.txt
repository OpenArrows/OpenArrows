cmake_minimum_required(VERSION 3.10)
project(DemocracyArrows LANGUAGES C VERSION 0.1.0)

# Libraries

# Vulkan is required to pre-compile GLSL shaders to SPIR-V (glslc)
find_package(Vulkan REQUIRED COMPONENTS glslc)

# ImageMagick is used to bake the texture atlas
# find_package(ImageMagick REQUIRED) # Commented out because it doesn't work on Windows (https://gitlab.kitware.com/cmake/cmake/-/issues/26772)

add_subdirectory(lib/glfw)

add_subdirectory(lib/glad/cmake glad_cmake)
glad_add_library(glad REPRODUCIBLE EXCLUDE_FROM_ALL LOADER API gl:core=4.3)

add_subdirectory(lib/cglm)
include_directories(lib/cglm/include)

include_directories(build)

# Pre-compile shaders
set(GLSLC_FLAGS -DGLSL --target-env=opengl)

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

# Game resources (textures, etc.)
add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/textures/arrows"
    COMMAND magick mogrify -format png -path "${PROJECT_BINARY_DIR}/textures/arrows" -background none "${CMAKE_SOURCE_DIR}/res/textures/arrows/arrow*.svg"
    DEPENDS res/textures/arrows
    VERBATIM)

add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/textures/arrows/atlas.png"
    COMMAND magick montage -mode concatenate -background none -tile 8x8 -geometry 256x256 "${PROJECT_BINARY_DIR}/textures/arrows/arrow*.png" "${PROJECT_BINARY_DIR}/textures/arrows/atlas.png"
    DEPENDS "${PROJECT_BINARY_DIR}/textures/arrows"
    VERBATIM)

add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/textures/arrows/atlas.dat"
    COMMAND magick "${PROJECT_BINARY_DIR}/textures/arrows/atlas.png" -depth 8 -colorspace RGB -background none -flatten -size 2048x2048 RGBA:${PROJECT_BINARY_DIR}/textures/arrows/atlas.dat
    DEPENDS "${PROJECT_BINARY_DIR}/textures/arrows/atlas.png"
    VERBATIM)

add_custom_target(resources ALL DEPENDS "${PROJECT_BINARY_DIR}/textures/arrows/atlas.dat")

# Game executable
set(SOURCES
    src/main.c
    src/game-map.c
    src/gl-debug.c)

add_executable(OpenArrows ${SOURCES})
add_dependencies(OpenArrows shaders)
add_dependencies(OpenArrows resources)
target_link_libraries(OpenArrows PUBLIC glfw glad)
set_property(TARGET OpenArrows PROPERTY C_STANDARD 23)

install(TARGETS OpenArrows)