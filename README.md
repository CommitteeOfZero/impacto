# Impacto

Impacto is an open-source generic MAGES. engine reimplementation written using C++ and OpenGL. It allows to run games using MAGES. engine using the original resources. The code is made to be portable and currently supports Windows/Linux/Nintendo Switch (homebrew).

## Status
Impacto is still in development, it supports the basic features of MAGES. engine, such as script parsing, displaying dialogue text, 2D/3D backgrounds and characters, system message boxes, choice selections, progressing through text, audio playback. However, it does not yet support such features as backlog, progress saving/loading, game-specific features, video playback, 2D effects and more.

## Games
Currently Impacto supports these MAGES. engine games:

* Memories Off 6 ~T-wave~
* Memories Off 6 Next Relation
* Chaos;Head Love Chu Chu
* Steins;Gate Hiyoku Renri no Darling
* Robotics;Notes Elite
* Robotics;Notes DaSH

## Building
For building on Windows with Visual Studio 2019 and newer, please refer to the [building instructions](doc/vs2019_build.md).

For building on Linux make sure you have installed all dependecies, such as libsdl2-dev, libopenal-dev, libogg-dev, libvorbis-dev, zlib1g-dev, libglm-dev. You will also have to compile and install libatrac9 manually.
After all dependecies are satisfied, in the repository root folder run:
```
cmake .
make
```

## License
Impacto source code is [ISC licensed](LICENSE), but due to inclusion of OpenAL-Soft, the binaries fall under GPLv2.