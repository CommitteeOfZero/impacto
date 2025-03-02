#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#define _USE_MATH_DEFINES
#include <math.h>

#include "config.h"

#include <SDL.h>

#ifndef IMPACTO_DISABLE_OPENGL
#include <glad/glad.h>
#endif

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#endif