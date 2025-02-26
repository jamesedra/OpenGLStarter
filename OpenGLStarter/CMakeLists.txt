cmake_minimum_required(VERSION 3.10)
project(OpenGLStarter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin/x64/cmake_build)

include_directories(${CMAKE_SOURCE_DIR})

include_directories(
  "C:/VSLibraries/glfw-3.4/LibInclude/include"
  "C:/VSLibraries/glfw-3.4/LibInclude"
)

link_directories("C:/VSLibraries/glfw-3.4/LibInclude")

add_library(common STATIC
    ${CMAKE_SOURCE_DIR}/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/shaders/shader.cpp
    ${CMAKE_SOURCE_DIR}/src/stb/stb_image.cpp
)

set(EXECUTABLES
    app_test
    app_msaa_post_process
    app_asteroid_belt
    app_normals_shading
    app_geometry_explosion
    app_framebuffer
    app_cubemapping
    app_depth_stencil
    app_blending
    app_model_loading
    app_light_casting
)

set(EXE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/7.advanced_lighting/blinn_shading.cpp
    ${CMAKE_SOURCE_DIR}/src/6.anti_aliasing/msaa_test_refactored.cpp
    ${CMAKE_SOURCE_DIR}/src/5.advanced_instancing/asteroid_belt_instanced.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/normals_visualization.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/geometry_explosion.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/framebuffer_test.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/cubemapping.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/depth_stencil_testing.cpp
    ${CMAKE_SOURCE_DIR}/src/4.advanced_testing/blending.cpp
    ${CMAKE_SOURCE_DIR}/src/3.model_loading/model_loading.cpp
    ${CMAKE_SOURCE_DIR}/src/2.lighting_systems/2.2.materials/multiple_lights.cpp
)

list(LENGTH EXECUTABLES execCount)
list(LENGTH EXE_SOURCES sourceCount)

message(STATUS "Executable count: ${execCount}")
message(STATUS "Source count: ${sourceCount}")

if(NOT execCount EQUAL sourceCount)
    message(FATAL_ERROR "Executable names and sources lists must have the same number of entries!")
endif()

set(ASSIMP_EXE "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp.exe")
set(ASSIMP_VIEWER_EXE "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp_viewer.exe")
set(ASSIMP_DLL "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp-vc143-mt.dll")
set(D3D_COMP "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/D3DCompiler_42.dll")
set(D3DX9_42 "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/D3DX9_42.dll")
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/src/resources")

math(EXPR LAST_INDEX "${execCount} - 1")

foreach(i RANGE 0 ${LAST_INDEX})
    list(GET EXECUTABLES ${i} exeName)
    list(GET EXE_SOURCES ${i} exeSource)

    add_executable(${exeName} ${exeSource})

    target_link_libraries(${exeName}
        common
        glfw3
        opengl32
        assimp-vc143-mt
    )

    add_custom_command(TARGET ${exeName} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_EXE}" $<TARGET_FILE_DIR:${exeName}>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_VIEWER_EXE}" $<TARGET_FILE_DIR:${exeName}>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_DLL}" $<TARGET_FILE_DIR:${exeName}>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${D3D_COMP}" $<TARGET_FILE_DIR:${exeName}>
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${D3DX9_42}" $<TARGET_FILE_DIR:${exeName}>
    )

    add_custom_command(TARGET ${exeName} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_DIR}"
        $<TARGET_FILE_DIR:${exeName}>/src/shaders
    )

    add_custom_command(TARGET ${exeName} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${RESOURCES_DIR}"
        "$<TARGET_FILE_DIR:${exeName}>/src/resources"
    )
endforeach()
