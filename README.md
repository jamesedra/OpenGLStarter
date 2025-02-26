# OpenGL Starter
This repository contains my learning outputs from Joey De Vries' _Learn OpenGL - Graphics Programming_ from Chapters 1 to 4. Each chapter focuses on different topics in graphics programming, from setting up an environment, camera, up to basic lighting, model loading, and advanced OpenGL concepts.

<img src="/saved/cube_mapping.PNG" alt="Cube Mapping Example" style="width: 50%"> 

## Dependencies
The project uses several external libraries:
- [GLFW 3.4](https://www.glfw.org/)
- [Assimp](https://github.com/assimp/assimp)
- OpenGL
- GLAD
- stb

The project does not have a vcpkg setup, so you will need to configure these dependencies manually.

## Build and Installation
The project uses CMake as its build system. Although libraries and includes are placed in a custom manner. If you would like to build the executable files, you will have to change the paths at _OpenGLStarter/CMakeLists.txt_. Particularly `include_directories`, `link_directories` and the Assimp files.

If you manage to set it correctly, create a build directory and run CMake:
``` bash
    mkdir build && cd build
    cmake ..
```
Then build the project.
``` bash
    cmake --build .
```
Below are some outputs from the built executable files.

### model instancing, anti-aliasing, and laplacian filter
<img src="/saved/asteroid_belt.PNG" alt="" style="width: 50%"><img src="/saved/outlines.PNG" alt="" style="width: 50%"> 

### phong shading, multiple light casting
<img src="/saved/light_casting.PNG" alt="" style="width: 50%"><img src="/saved/spot_light.PNG" alt="" style="width: 50%">  

### model stencil outlining, explosion, and normals visualization
<img src="/saved/model_loading.PNG" alt="" style="width: 33%"><img src="/saved/normals.PNG" alt="" style="width: 33%"><img src="/saved/model_explosion.PNG" alt="" style="width: 33%">

