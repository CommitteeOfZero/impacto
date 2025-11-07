#pragma once

#include <cstdint>
#include <cstdlib>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>

#include "config.h"

#include <SDL.h>

#ifndef IMPACTO_DISABLE_OPENGL
#if __VITA__
#include <vitaGL.h>
#else
#include <glad/glad.h>
#endif
#endif

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#endif

#include <fmt/base.h>
#include <fmt/std.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/chrono.h>
#include <ankerl/unordered_dense.h>