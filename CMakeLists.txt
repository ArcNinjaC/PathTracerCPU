# CMakeList.txt : CMake project for PP_RayTracer1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("PP_RayTracer1")

add_subdirectory(externals/SDL-release-2.30.9)
add_subdirectory(externals/SDL_image-release-2.8.2)

# Add source to this project's executable.
add_executable (PP_RayTracer1 "Source.cpp" "Vector.h" "Camera.h" "Ray.h" "Object.h" "Renderer.h")
target_link_libraries(PP_RayTracer1 PRIVATE 
    SDL2
    SDL2_image
)


if (WIN32)
	target_link_libraries(PP_RayTracer1 PRIVATE SDL2main)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PP_RayTracer1 PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(
  TARGET PP_RayTracer1 POST_BUILD COMMAND
  cmake -E copy_if_different
    "$<TARGET_FILE:SDL2>"
    "$<TARGET_FILE:SDL2_image>" 
    $<TARGET_FILE_DIR:PP_RayTracer1>
    VERBATIM
)

# TODO: Add tests and install targets if needed.
