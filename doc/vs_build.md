# Building with Visual Studio

## Dependencies

Visual Studio must have the "Desktop development with C++" optional features installed (you can check if you have them installed by going into the "Tools" menu and selecting "Get Tools and Features...").

Git command-line tool must be available from within PowerShell. If you don't have it installed, you can download it from https://git-scm.com/downloads

Enable execution of PowerShell scripts by running the following command in an elevated PowerShell prompt:

`Set-ExecutionPolicy RemoteSigned`

- If running on a version of Visual Studio older than 2022 (or vcpkg isn't installed), make sure to set the environment variable `VCPKG_ROOT` to `${impactoDir}/build/vcpkg`, or wherever else your vcpkg root directory is located.

Run `.\build-deps.ps1` from a PowerShell prompt, addings args for architecture [`x64`, `x86`] if needed

## Visual Studio CMake project setup
- You may have to enable CMakePresets in VS Options -> Cmake

- The included `CMakePresets.json` in the repository root should already have several default build configurations. If you need to make any changes, create a `CMakeUserPresets.json`, of the same format, in the repository root (don't worry, it's gitignored) and change paths if necessary (the directories will be created automatically upon building). Make sure to name the presets differently from the default presets, since user presets cannot override existing presets.

- Open the project with *File->Open->Folder...* in Visual Studio
- When picking a startup item, make sure to use `impacto.exe (Install)`

## Cry

about the absolute state of C++ dependency management