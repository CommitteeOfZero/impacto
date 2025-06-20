# Build instructions based on Ubuntu 20.04 LTS

You can use the following shell script for getting dependencies and setting up first time build on Ubuntu.

First time setup for installing dependencies, vcpkg, and cloning impacto.   

```shell
# Build instructions based on Ubuntu 20.04 LTS

# Install required programs and dependencies
sudo apt update
sudo apt -y install git cmake ninja-build build-essential libstdc++6 nasm curl zip pkg-config autoconf libtool libx11-dev libxft-dev libxext-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev libibus-1.0-dev xcb libxrandr-dev
# Note: The CMake version available from your system package manager may be out of date, check the version with "cmake --version", 
# and if it's lower than the version in CMakeLists, update it to match.

# Clone vcpkg
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
# Set VCPKG_ROOT env var on user startup, needed for cmake to find the vcpkg toolchain
echo 'export VCPKG_ROOT="$(pwd)/vcpkg"' >> ~/.profile
source ~/.profile

# Clone impacto 
git clone https://github.com/CommitteeOfZero/impacto.git
```

Once this is done, from now on you can just enter the impacto directory and build using CMake.
```shell
cd impacto
# for building release /w symbols build
cmake --preset Release && cmake --build --preset x64-Release
# for building debug build
cmake --preset Debug && cmake --build --preset x64-Debug
```
Once built, files should be installed to the `install` directory. Follow instructions in [getting_started](doc/getting_started.md) on where to place game files and resources.

You can also customize additional CMake options by creating a file called `CMakeUserPresets.json` (it's gitignored)
based off of the existing `CMakePresets.json`. This will allow you to customize settings such as enabling warnings, install directory. 
Make sure preset names are different from the original presets. 