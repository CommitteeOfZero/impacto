#!/bin/sh

emcmake cmake -DCMAKE_FIND_ROOT_PATH=/impacto-deps -DCMAKE_PREFIX_PATH=/local $@ /src
emmake make