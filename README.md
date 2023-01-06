# micro Vulkan Toy

**(This repo is a vulkan rendering toy based on [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine#Basics))**

A small computer graphics learning project for [Vulkan®](https://www.khronos.org/vulkan/), based on tutorial project [littleVulkanEngine](https://github.com/blurrypiano/littleVulkanEngine#Basics)). For some toy feature experiments.

## Table of Contents

- [Features](#Features)
  - [PMX model](#PMXLoad)
- [Building](#Building)
  - [Building for Unix](#UnixBuild)
  - [Building for MacOS](#MacOSBuild)
  - [Building for Windows](#WindowsBuild)

- [Credits and Attributions](#CreditsAttributions)

## <a name="Building"></a> Building

### <a name="UnixBuild"></a> Unix Build Instructions

- Install the dependencies: cmake, glm, vulkan and glfw

- For example
  ```
    sudo apt install vulkan-tools
    sudo apt install libvulkan-dev
    sudo apt install vulkan-validationlayers-dev spirv-tools
    sudo apt install libglfw3-dev
    sudo apt install libglm-dev
    sudo apt install cmake
  ```
- To Build
  ```
   cd LittleVulkanEngine
   ./unixBuild.sh
  ```

### <a name="MacOSBuild"></a> MacOS Build Instructions

#### Install Dependencies

- [Download and install MacOS Vulkan sdk](https://vulkan.lunarg.com/)
- [Download and install Homebrew](https://brew.sh/)

- Then in a terminal window

  ```
    brew install cmake
    brew install glfw
    brew install glm
  ```

- To Build
  ```
   cd littleVulkanEngine
   ./unixBuild.sh
  ```

### <a name="WindowsBuild"></a> Windows build instructions

- [Download and install Windows Vulkan sdk](https://vulkan.lunarg.com/)
- [Download and install Windows cmake x64 installer](https://cmake.org/download/)
  - Make sure to select "Add cmake to the system Path for all users"
- [Download GLFW](https://www.glfw.org/download.html) (64-bit precompiled binary)
- [Download GLM](https://github.com/g-truc/glm/releases)
- Download and open the project and rename "envWindowsExample.cmake" to ".env.cmake"
- Update the filepath variables in .env.cmake to your installation locations

#### Building for Visual Studio 2019

- In windows powershell

```
 cd littleVulkanEngine
 mkdir build
 cmake -S . -B .\build\
```

- If cmake finished successfully, it will create a LveEngine.sln file in the build directory that can be opened with visual studio. In visual studio right click the Shaders project -> build, to build the shaders. Right click the LveEngine project -> set as startup project. Change from debug to release, and then build and start without debugging.

#### Building for minGW

- [Download and install MinGW-w64](https://www.mingw-w64.org/downloads/), you probably want MingW-W64-builds/
- Make sure MinGW has been added to your Path
- Also set MINGW_PATH variable in the project's .env.cmake
- In windows powershell

```
 cd littleVulkanEngine
 ./mingwBuild.bat
```

- This will build the project to build/LveEngine.exe, double click in file explorer to open and run

## <a name="Features"></a> Features

### <a name="PMXLoad"></a> PMX model

This feature is under develepment.




## <a name="CreditsAttributions"></a> Credits

Thanks to the authors of these libraries :

- [OpenGL Mathematics (GLM)](https://github.com/g-truc/glm)

Thanks to [LunarG](http://www.lunarg.com)

Thanks to the wonderful opensource examples by [Sascha Willems](https://github.com/SaschaWillems/Vulkan)

Thanks to [ThinMatrix](https://www.youtube.com/user/ThinMatrix/featured) and his wonderful OpenGL game tutorial series which was a huge inspiration for this series and how I first started learning computer graphics

Thanks to [Sean Plott](https://day9.tv/) and the #DK30 challenge, for providing the motivating kick to give this a shot

## Attributions / Licenses

- Vulkan and the Vulkan logo are trademarks of the [Khronos Group Inc.](http://www.khronos.org)
