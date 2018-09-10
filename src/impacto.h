#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "config.h"

// TODO own _malloca for gcc

#if defined(_malloca)
#define ImpStackAlloc _malloca
#define ImpStackFree _freea
#else
#define ImpStackAlloc malloc
#define ImpStackFree free
#endif