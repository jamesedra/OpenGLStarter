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

add_executable(app_test 
    ${CMAKE_SOURCE_DIR}/src/7.advanced_lighting/blinn_shading.cpp
    ${CMAKE_SOURCE_DIR}/src/glad.c
    ${CMAKE_SOURCE_DIR}/src/camera.cpp
    ${CMAKE_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/Model.cpp
    ${CMAKE_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/shaders/shader.cpp
    ${CMAKE_SOURCE_DIR}/src/stb/stb_image.cpp
)

target_link_libraries(app_test
  glfw3
  opengl32
  assimp-vc143-mt
)

set(ASSIMP_EXE "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp.exe")
set(ASSIMP_VIEWER_EXE "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp_viewer.exe")
set(ASSIMP_DLL "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/assimp-vc143-mt.dll")
set(D3D_COMP "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/D3DCompiler_42.dll")
set(D3DX9_42 "${CMAKE_SOURCE_DIR}/../bin/x64/Debug/D3DX9_42.dll")

add_custom_command(TARGET app_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_EXE}" $<TARGET_FILE_DIR:app_test>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_VIEWER_EXE}" $<TARGET_FILE_DIR:app_test>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ASSIMP_DLL}" $<TARGET_FILE_DIR:app_test>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${D3D_COMP}" $<TARGET_FILE_DIR:app_test>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${D3DX9_42}" $<TARGET_FILE_DIR:app_test>
)

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/src/shaders")
add_custom_command(TARGET app_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${SHADER_DIR}"
    $<TARGET_FILE_DIR:app_test>/src/shaders
)
