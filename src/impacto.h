#pragma once

#include <cstdint>
#include <cstdlib>
#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>

#include "config.h"

#include <SDL3/SDL.h>

#if defined(IMPACTO_RENDERER_OPENGL) || defined(IMPACTO_RENDERER_OPENGLES)
#include <glad/glad.h>
#endif

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#endif

#include <fmt/base.h>
#include <fmt/std.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/chrono.h>
#include <ankerl/unordered_dense.h>
#include <magic_enum/magic_enum.hpp>