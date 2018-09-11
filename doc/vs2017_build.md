# Building with Visual Studio 2017

Here's what I personally do to build this with Visual Studio 2017.

YMMV. I had no idea what I was doing. This is definitely not the best way, but it works /shrug.

## Dependencies

Create a folder somewhere on your drive, `impacto-deps`.

### GLEW

- Grab Windows binaries from http://glew.sourceforge.net/
- Extract to `impacto-deps`. You should now have `impacto-deps/glew-2.1.0/LICENSE.txt` etc.

### SDL2

- Grab Visual C++ binaries (`SDL2-devel-2.0.8-VC.zip`) from https://www.libsdl.org/download-2.0.php
- Extract to `impacto-deps`. You should now have `impacto-deps/SDL2-2.0.8/README.txt` etc.

### glm

- Grab the distribution .zip (e.g. `glm-0.9.9.1.zip`) from https://github.com/g-truc/glm/releases/latest
- Extract to `impacto-deps` and rename the contained `glm` folder, e.g. to `glm-0.99.1`. You should now have `impacto-deps/glm-0.99.1/readme.md` etc.
- Open CMake GUI
- Source code => `C:/path/to/impacto-deps/glm-0.99.1`
- Binaries => `C:/path/to/impacto-deps/glm-build`
- *Configure*
- Select *Visual Studio 15 2017 Win64*
- Set `CMAKE_INSTALL_PREFIX` to `C:/path/to/impacto-deps/glm`
- *Configure* again
- *Generate*
- *Open Project*
- Build **ALL_BUILD** project, wait
- Build **INSTALL** project

### ZLIB

- Download latest source .zip from https://github.com/madler/zlib/releases
- Extract to `impacto-deps`. You should now have `impacto-deps/zlib-1.2.11/.gitignore` etc.
- Open CMake GUI
- Source code => `impacto-deps/zlib-1.2.11`
- Binaries => `C:/path/to/impacto-deps/zlib-build`
- *Configure*
- Select *Visual Studio 15 2017 Win64*
- Set `CMAKE_INSTALL_PREFIX` to `C:/path/to/impacto-deps/ZLIB`, change the other filled in paths accordingly (e.g. `INSTALL_BIN_DIR` => `C:/path/to/impacto-deps/ZLIB/bin`)
- *Configure* again
- *Generate*
- *Open Project*
- Build **ALL_BUILD** project, wait
- Build **INSTALL** project

## Visual Studio 2017 CMake project setup

- Put following `CMakeSettings.json` in the repository root (don't worry, it's gitignored) and change paths accordingly:

```json
{
  "configurations": [
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "RelWithDebInfo",
      "inheritEnvironments": [
        "msvc_x64_x64"
      ],
      "environments": [
        {
          "SDL2DIR": "C:\\path\\to\\impacto-deps\\SDL2-2.0.8",
          "CMAKE_LIBRARY_PATH": "C:\\path\\to\\impacto-deps\\glew-2.1.0\\lib\\Release\\x64",
          "CMAKE_PREFIX_PATH": "C:\\path\\to\\impacto-deps"
        }
      ],
      "variables": [
        {
          "name": "GLEW_INCLUDE_DIR",
          "value": "C:\\path\\to\\impacto-deps\\glew-2.1.0\\include"
        },
        {
          "name": "ZLIB_ROOT",
          "value": "C:\\path\\to\\impacto-deps\\ZLIB"
        }
      ],
      "buildRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\build\\${name}",
      "installRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": ""
    },
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "inheritEnvironments": [
        "msvc_x64_x64"
      ],
      "environments": [
        {
          "SDL2DIR": "C:\\path\\to\\impacto-deps\\SDL2-2.0.8",
          "CMAKE_LIBRARY_PATH": "C:\\path\\to\\impacto-deps\\glew-2.1.0\\lib\\Release\\x64",
          "CMAKE_PREFIX_PATH": "C:\\path\\to\\impacto-deps"
        }
      ],
      "variables": [
        {
          "name": "GLEW_INCLUDE_DIR",
          "value": "C:\\path\\to\\impacto-deps\\glew-2.1.0\\include"
        },
        {
          "name": "ZLIB_ROOT",
          "value": "C:\\path\\to\\impacto-deps\\ZLIB"
        }
      ],
      "buildRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\build\\${name}",
      "installRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": ""
    }
  ]
}
```

- Open the project with *File->Open->Folder...* in VS2017
- When picking a build target, make sure to use `impacto.exe (Install)`

## Cry

about the absolute state of C++ dependency management