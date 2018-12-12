#   docker build -t emscripten:latest --build-arg EMSCRIPTEN_SDK=sdk-tag-1.38.21-64bit https://raw.githubusercontent.com/trzecieu/emscripten-docker/master/docker/trzeci/emscripten/Dockerfile
#   docker build -t impacto-emscripten .
# (after 1.38.22 we should switch to trzeci/emscripten:latest)
#   docker run --rm -v /path/to/build/directory:/build -v /path/to/impacto/repository/root:/src -u emscripten impacto-emscripten

FROM emscripten:latest

USER emscripten

RUN echo 'Caching emscripten ports' \
    && touch /tmp/ports_cache.cpp \
    && emcc -s USE_SDL=2 -s USE_OGG=1 -s USE_VORBIS=1 -s USE_ZLIB=1 /tmp/ports_cache.cpp -o /tmp/ports_cache.js \
    && rm /tmp/ports_cache.*

USER root    
RUN mkdir -p /impacto-deps/local/include \
    && mkdir -p /impacto-deps/local/lib \
    && chown -R emscripten /impacto-deps
USER emscripten
    
COPY libatrac9-emscripten.mk /impacto-deps
    
RUN echo 'Building libatrac9' \
    && mkdir -p /tmp/libatrac9-build \
    && cd /tmp/libatrac9-build \
    && git clone https://github.com/Thealexbarney/LibAtrac9 \
    && cd LibAtrac9 \
    && git checkout 6a9e00f6c7abd74d037fd210b6670d3cdb313049 \
    && cd C \
    && emmake make -f /impacto-deps/libatrac9-emscripten.mk \
    && cp bin/libatrac9.a /impacto-deps/local/lib/ \
    && mkdir -p /impacto-deps/local/include/libatrac9 \
    && cp src/libatrac9.h /impacto-deps/local/include/libatrac9/ \
    && cd /src \
    && rm -rf /tmp/libatrac9-build
    
RUN echo 'Building glm' \
    && mkdir -p /tmp/glm-build \
    && cd /tmp/glm-build \
    && wget https://github.com/g-truc/glm/releases/download/0.9.9.3/glm-0.9.9.3.zip \
    && unzip -q glm-0.9.9.3.zip \
    && mkdir build \
    && cd build \
    && emcmake cmake -DCMAKE_INSTALL_PREFIX=/impacto-deps/local -DGLM_TEST_ENABLE=OFF ../glm \
    && emmake make \
    && emmake make install \
    && cd /src \
    && rm -rf /tmp/glm-build

VOLUME /build
WORKDIR /build

CMD ["/src/docker/impacto-emscripten/build_emscripten.sh"]