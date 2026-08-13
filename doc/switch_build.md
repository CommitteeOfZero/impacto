# Building for Nintendo Switch (Horizon / devkitPro)

There are two types of dependencies: Deps built from source from vcpkg builds and cross-compiled 
with the devkitPro toolchain and deps provided by **devkitPro portlibs** as system packages.
System deps are instead shipped as *overlay ports* under [`../portfiles-switch/`](../portfiles-switch)
For libraries devkitPro ships without a CMake config, the overlay installs a
tiny `*Config.cmake` wrapper around the devkitPro pkg-config, so impacto uses 
the same `find_package(... CONFIG)` interface and imported targets on every platform. 
## Prerequisites

* [**devkitPro** with the devkitA64 toolchain](https://devkitpro.org/wiki/Getting_Started), 
  * `libnx`, and the Switch portlibs
    (`switch-dev` group plus `switch-sdl2`, `switch-zlib`, `switch-libogg`,
    `switch-libvorbis`, `switch-libwebp`, `switch-ffmpeg`, `switch-dav1d`,
    `switch-libass`, and `switch-openal-soft`). Install with `dkp-pacman`.
  * On Windows, WSL is recommended, since msys paths can be funky.
* The `DEVKITPRO` environment variable pointing at your devkitPro install
  (e.g. `/opt/devkitpro`).
* A **vcpkg** checkout, with the `VCPKG_ROOT` environment variable pointing at it.
* **CMake** ≥ 3.31 and **Ninja**.

## Building

From the repository root:

```bash
cmake --preset ci-release-switch
cmake --build --preset ci-release-switch
```

This configures with the vcpkg toolchain, the `arm64-switch` triplet (which
chainloads `HorizonNX.toolchain`), and the Switch overlay ports, then builds and
runs the `install` step. The staged build — the `impacto` ELF plus `shaders`,
`profiles`, `games`, and the `*.lua` config files by default are installed to
`release/ci-release-switch/`.
