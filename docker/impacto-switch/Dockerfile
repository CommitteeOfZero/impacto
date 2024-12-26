FROM devkitpro/devkita64

LABEL org.opencontainers.image.source https://github.com/committeeofzero/impacto

RUN apt install p7zip-full -y

# Install latest cmake
RUN echo "Removing older version of cmake" && \
  apt remove --purge --auto-remove -y cmake
ADD https://github.com/Kitware/CMake/releases/download/v3.31.3/cmake-3.31.3-linux-x86_64.sh /cmake-3.31.3-linux-x86_64.sh
RUN mkdir /opt/cmake
RUN sh /cmake-3.31.3-linux-x86_64.sh --prefix=/opt/cmake --skip-license
RUN ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake
RUN cmake --version