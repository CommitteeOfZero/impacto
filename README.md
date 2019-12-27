# Impacto

Impacto is an open-source generic MAGES. engine reimplementation written using C++ and OpenGL. It allows to run games using MAGES. engine using the original resources. The code is made to be portable and currently supports bilding for Windows/Linux/Nintendo Switch (homebrew).

## Status
**Impacto is still in development**, it gets ingame in several games but major functionality and architectural design is still missing, **no game is fully playable yet**. Refer to #placeholder for supported games and remaining work.

## Building
For building on Windows with Visual Studio 2019 and newer, please refer to the [building instructions](doc/vs2019_build.md).

For building on Linux make sure you have installed all dependecies, such as libsdl2-dev, libopenal-dev, libogg-dev, libvorbis-dev, zlib1g-dev, libglm-dev. You will also have to compile and install [libatrac9](https://github.com/Thealexbarney/LibAtrac9) manually.
After all dependecies are satisfied, in the repository root folder run:
```
cmake .
make
```

## Contributing
Contributing to development of Impacto is always welcome. Wether it's fixing something, adding new features or changing existing ones, feel free to submit a PR or file an issue. For details on how to get started please check out the [Getting Started](doc/getting_started.md) guide.

## License
Impacto source code is licensed under the [ISC license](LICENSE).