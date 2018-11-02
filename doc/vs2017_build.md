# Building with Visual Studio 2017

Here's what I personally do to build this with Visual Studio 2017.

YMMV. I had no idea what I was doing. This is definitely not the best way, but it works /shrug.

## Dependencies

Create a folder somewhere on your drive, `impacto-deps`.

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

### OpenAL Soft

- Grab binaries (e.g. `openal-soft-1.19.1-bin.zip`) from http://openal-soft.org/
- Extract to `impacto-deps`. You should now have `impacto-deps/openal-soft-1.19.1-bin/readme.txt` etc.

### Ogg

- Grab latest source .zip from https://github.com/xiph/ogg/releases (repo dump, not the manual packages)
- Extract to `ìmpacto-deps`. You should now have `impacto-deps/ogg-1.3.3/CMakeLists.txt` etc.
- Open CMake GUI
- Source code => `impacto-deps/ogg-1.3.3`
- Binaries => `C:/path/to/impacto-deps/ogg-build`
- *Configure*
- Select *Visual Studio 15 2017 Win64*
- Set `CMAKE_INSTALL_PREFIX` to `C:/path/to/impacto-deps/libogg`
- *Configure* again
- *Generate*
- *Open Project*
- Switch configuration to *RelWithDebInfo*
- Build **ALL_BUILD** project, wait
- Build **INSTALL** project

### Ogg Vorbis

- Grab latest source .zip from https://github.com/xiph/vorbis/releases
- Extract to `ìmpacto-deps`. You should now have `impacto-deps/vorbis-1.3.6/COPYING` etc.
- Open CMake GUI
- Source code => `impacto-deps/vorbis-1.3.6`
- Binaries => `C:/path/to/impacto-deps/vorbis-build`
- *Configure*
- Select *Visual Studio 15 2017 Win64*
- Set `OGG_INCLUDE_DIRS` to `C:/path/to/impacto-deps/libogg/include`
- Set `OGG_LIBRARIES` to `C:/path/to/impacto-deps/libogg/lib/ogg.lib`
- Set `CMAKE_INSTALL_PREFIX` to `C:/path/to/impacto-deps/vorbis`
- *Configure* again
- *Generate*
- *Open Project*
- Switch configuration to *RelWithDebInfo*
- Build **ALL_BUILD** project, wait
- Build **INSTALL** project

### Libatrac9

- Grab master from https://github.com/Thealexbarney/LibAtrac9 (tested with 6a9e00f)
- Extract somewhere
- Open `LibAtrac9-master/C/libatrac9.sln` in VS2017
- Switch configuration to *Release*
- Build
- Make folder `C:/path/to/impacto-deps/libatrac9` and subfolders `include/libatrac9`, `lib` and `bin`
- Copy `LibAtrac9-master/C/x64/Release/libatrac9.dll` to `C:/path/to/impacto-deps/libatrac9/bin`
- Copy `LibAtrac9-master/C/src/*.h` to `C:/path/to/impacto-deps/libatrac9/include/libatrac9`
- Copy `LibAtrac9-master/C/x64/Release/libatrac9.lib` to `C:/path/to/impacto-deps/libatrac9/lib`


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
          "OPENALDIR": "C:\\path\\to\\impacto-deps\\openal-soft-1.19.1-bin",
          "OGGDIR": "C:\\path\\to\\impacto-deps\\libogg",
          "VORBISDIR": "C:\\path\\to\\impacto-deps\\vorbis",
          "LIBATRAC9DIR": "C:\\path\\to\\impacto-deps\\libatrac9",
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
          "OPENALDIR": "C:\\path\\to\\impacto-deps\\openal-soft-1.19.1-bin",
          "OGGDIR": "C:\\path\\to\\impacto-deps\\libogg",
          "VORBISDIR": "C:\\path\\to\\impacto-deps\\vorbis",
          "LIBATRAC9DIR": "C:\\path\\to\\impacto-deps\\libatrac9",
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