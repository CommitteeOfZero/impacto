# impacto [![Build Status](https://dev.azure.com/mrrevo/impacto/_apis/build/status/CommitteeOfZero.impacto?branchName=master)](https://dev.azure.com/mrrevo/impacto/_build/latest?definitionId=3&branchName=master)

**impacto** is an open-source **reimplementation** of the **"MAGES." visual novel engine** in C++ and OpenGL. Using the original data files, impacto can run any supported game on any supported platform.

## Status

**impacto is in active development**. At current stage it should be possible to reach all endings in at least one supported game - "Memories Off 6 \~T-Wave\~." However, some functionality and architectural design are still missing and **no games are fully complete yet**. Refer to [#1](https://github.com/CommitteeOfZero/impacto/issues/1) for supported games and remaining work.

impacto is currently being developed for **64-bit Windows 10 and desktop Linux PCs** but we've successfully experimented on macOS, Android (ARM), Switch (homebrew and Linux) and HTML5/WebAssembly (via Emscripten) and full official Android support is planned.

## Building

For building on Windows with Visual Studio 2019 or newer, please refer to the [building instructions](doc/vs2019_build.md).

For building on Linux, see the [instructions for Ubuntu 18.04 Desktop](doc/ubuntu1804_build.md) and adapt to your distribution if necessary.

More platforms and toolchains are known to work.

## Contributing

**We are looking for contributors!** Check out the [Getting Started guide](doc/getting_started.md) for pointers on setting up games for testing, finding your way around the codebase and adding functionality. Also check out the [Contributor guide](doc/contributor_guide.md) for important information on the code style you should follow.

There is work to be done for C++ programmers of any skill level in a wide range of subjects, on game engine architecture (design and implementation), reverse-engineering the original (looking at just the game's outside behaviour as a *black box* or inspecting the internals through a *white box*), replicating it, improving on it with new functionality, documenting our efforts, fixing bugs and polish. 

If you're interested, [come join our Discord](https://discord.gg/rq4GGCh) to discuss ideas and help you get into it.

## Legal stuff

impacto source code as a whole is released under the liberal [ISC license](LICENSE). Some parts are based on or copied from third-party code under various licenses. Binary distributions or parts thereof may fall under more restrictive licensing terms. See [THIRDPARTY.md](THIRDPARTY.md) for details and attribution.
