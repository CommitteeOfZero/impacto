#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "config.h"

#include <glad/glad.h>

#include <SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "../vendor/nuklear/nuklear.h"