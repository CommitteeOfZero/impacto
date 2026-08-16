# Switch (Horizon / devkitPro) overlay ports

These overlay ports are used only by the `arm64-switch` triplet (via the
`ci-release-switch` CMake preset, which prepends this directory to
`VCPKG_OVERLAY_PORTS`). They shadow the upstream ports only for the Switch build, so
desktop/mobile builds keep using the real upstream ports.

Their job is to satisfy impacto's `vcpkg.json` dependency graph *without* vcpkg
rebuilding libraries that devkitPro already provides — the technique from
<https://devblogs.microsoft.com/cppblog/using-system-package-manager-dependencies-with-vcpkg/>.
Non-system dependencies (e.g. `fmt`) are **not** overlaid; vcpkg builds those from
source for the Switch triplet just like on desktop.

The overlays come in three flavours.

## 1. Pass-through overlays

`portfile.cmake` is just `set(VCPKG_POLICY_EMPTY_PACKAGE enabled)` — vcpkg installs
nothing and CMake's existing lookup already finds the devkitPro copy:

| Port          | Resolved by                                                                                            |
|---------------|--------------------------------------------------------------------------------------------------------|
| `zlib`        | `find_package(ZLIB)` (CMake module) → devkitPro                                                        |
| `openal-soft` | `find_package(OpenAL)` (CMake module) → devkitPro                                                      |
| `libass`      | `pkg_check_modules(libass)` in impacto's CMakeLists                                                    |
| `vulkan`      | unused on Switch (`IMPACTO_DISABLE_VULKAN`)                                                            |

## 2. Config-emitting overlays

devkitPro ships these libraries with `.pc` files but **no CMake package config**, so a
pass-through overlay would force impacto to keep a Switch-specific `pkg_check_modules`
branch. Instead, each `portfile.cmake` installs a small `*Config.cmake` wrapper (still
`VCPKG_POLICY_EMPTY_PACKAGE`, no binaries) that locates the devkitPro library via
pkg-config and re-exports it as the **same imported targets the other platforms use**:

| Port        | Config installed  | Targets provided                                   |
|-------------|-------------------|----------------------------------------------------|
| `libogg`    | `share/ogg/OggConfig.cmake`     | `Ogg::ogg`                            |
| `libvorbis` | `share/vorbis/VorbisConfig.cmake` | `Vorbis::vorbis`, `Vorbis::vorbisfile` |
| `libwebp`   | `share/webp/WebPConfig.cmake`   | `WebP::webp`, `WebP::webpdecoder`, `WebP::webpdemux` |
| `freetype`  | `share/freetype/FreetypeConfig.cmake` | `Freetype::Freetype`             |
| `harfbuzz`  | `share/harfbuzz/harfbuzzConfig.cmake` | `harfbuzz::harfbuzz`             |

This is what lets impacto's `CMakeLists.txt` call `find_package(Ogg CONFIG)` /
`find_package(Vorbis CONFIG)` / `find_package(WebP CONFIG)` and link `Ogg::ogg`,
`Vorbis::vorbis`, etc. on **every** platform, with no `if(NX)` dependency branch. The
only Switch-specific link libraries left in the CMakeLists are the libnx windowing /
GPU driver libs (`EGL`, `glapi`, `drm_nouveau`, `nx`), which are not vcpkg-managed
dependencies.

## 3. Source-built overlays

devkitPro's prebuilt copies aren't usable here, so these build the real thing from
source via `vcpkg_from_github` + `vcpkg_cmake_configure`/`install`:

| Port          | Why                                                                                                     |
|---------------|-----------------------------------------------------------------------------------------------------------|
| `sdl3`        | No prebuilt devkitPro portlib; builds `devkitPro/SDL@switch-sdl-3.4` with `NINTENDO_SWITCH=ON`.         |
| `openal-soft` | devkitPro's prebuilt copy only has an SDL2 backend, incompatible with SDL3. Builds `kcat/openal-soft` with `ALSOFT_BACKEND_SDL3=ON`. |
