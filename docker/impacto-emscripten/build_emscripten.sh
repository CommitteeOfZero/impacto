#!/bin/sh

emcmake cmake -DCMAKE_FIND_ROOT_PATH=/impacto-deps -DCMAKE_PREFIX_PATH=/local $@ /src
emmake make
# https://github.com/kripken/emscripten/issues/5436
sed -i 's/$legalf32//g' impacto.js