#pragma once

#define NOINSTRUMENT __attribute__((no_instrument_function))

#ifdef __cplusplus
extern "C" {
#endif

void enableProbing() NOINSTRUMENT;
void endProbing() NOINSTRUMENT;

#ifdef __cplusplus
}
#endif

