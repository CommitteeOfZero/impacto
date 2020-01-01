# Building with Visual Studio 2019

## Dependencies

Visual Studio must have the "Desktop development with C++" optional features installed (you can check if you have them installed by going into the "Tools" menu and selecting "Get Tools and Features...").

Git command-line tool must be available from within PowerShell. If you don't have it installed, you can download it from https://git-scm.com/downloads

Enable execution of PowerShell scripts by running the following command in an elevated PowerShell prompt:

`Set-ExecutionPolicy RemoteSigned`

Run `.\build-deps.ps1` from a PowerShell prompt.

## Visual Studio 2019 CMake project setup

- Put following `CMakeSettings.json` in the repository root (don't worry, it's gitignored) and change paths accordingly (the directories will be created automatically upon building):

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
      "buildRoot": "C:\\full\\path\\to\\impacto-build\\build\\${name}",
      "installRoot": "C:\\full\path\\to\\impacto-build\\install\\${name}",
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
      "buildRoot": "C:\\full\\path\\to\\impacto-build\\build\\${name}",
      "installRoot": "C:\\full\\path\\to\\impacto-build\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": ""
    }
  ]
}
```

- Open the project with *File->Open->Folder...* in VS2019
- When picking a startup item, make sure to use `impacto.exe (Install)`

## Cry

about the absolute state of C++ dependency management