# Building with Visual Studio 2019

## Dependencies

Visual Studio must have the "Desktop development with C++" optional features installed (you can check if you have them installed by going into the "Tools" menu and selecting "Get Tools and Features...").

Git command-line tool must be available from within PowerShell. If you don't have it installed, you can download it from https://git-scm.com/downloads

Enable execution of PowerShell scripts by running the following command in an elevated PowerShell prompt:

`Set-ExecutionPolicy RemoteSigned`

Run `.\build-deps.ps1` from a PowerShell prompt.

## Visual Studio 2019 CMake project setup

- Put following `CMakePresets.json` in the repository root (don't worry, it's gitignored).
## CMakePresets.json for VSCode or other IDEs

```json
{
  "version": 3,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 11
  },
  "configurePresets": [
    {
      "name": "x64-Release",
      "description": "x64 Release",
      "hidden": false,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/impacto-build/build/${presetName}",
      // Include this if vcpkg was locally cloned
      // "environment": {
      //   "VCPKG_ROOT": "${sourceDir}/impacto/build/vcpkg/"
      // },
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "RelWithDebInfo",
        "CMAKE_TOOLCHAIN_FILE": "${env:VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_PREFIX_PATH": "${sourceDir}/build/vcpkg/installed/x64-windows/",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "YES",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/impacto-build/build/${presetName}",
        "CMAKE_C_COMPILER": "cl.exe",
        "CMAKE_CXX_COMPILER": "cl.exe",
        "VCPKG_INSTALLED_DIR": "${sourceDir}/vcpkg_installed"
      }
    },
    {
      "name": "x64-Debug",
      "description": "x64 Debug",
      "hidden": false,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/impacto-build/build/${presetName}",
      // Include this if vcpkg was locally cloned
      // "environment": {
      //   "VCPKG_ROOT": "${sourceDir}/impacto/build/vcpkg/"
      // },
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_TOOLCHAIN_FILE": "${env:VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_PREFIX_PATH": "${sourceDir}/build/vcpkg/installed/x64-windows/",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "YES",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/impacto-build/build/${presetName}",
        "CMAKE_C_COMPILER": "cl.exe",
        "CMAKE_CXX_COMPILER": "cl.exe",
        "VCPKG_INSTALLED_DIR": "${sourceDir}/vcpkg_installed"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "x64-Release",
      "description": "x64 Release",
      "configurePreset": "x64-Release",
      "targets": [
        "install"
      ]
    },
    {
      "name": "x64-Debug",
      "description": "x64 Debug",
      "configurePreset": "x64-Debug",
      "targets": [
        "install"
      ]
    }
  ]
}
```

- Open the project with *File->Open->Folder...* in VS2019
- When picking a startup item, make sure to use `impacto.exe (Install)`

## Cry

about the absolute state of C++ dependency management