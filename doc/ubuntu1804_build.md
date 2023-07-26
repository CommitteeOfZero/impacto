# Build instructions based on Ubuntu 18.04 LTS

You can use the following shell script for getting dependencies and setting up first time build on Ubuntu 18.04 LTS.

```shell
# Build instructions based on Ubuntu 18.04 LTS

# Install required programs and dependencies
# Also install libglm-dev if the glm version provided by your distro is 0.9.9.3 or higher
sudo apt-get -y install git cmake libsdl2-dev libopenal-dev libogg-dev libvorbis-dev zlib1g-dev

# Clone impacto 
git clone https://github.com/CommitteeOfZero/impacto.git
cd impacto

# Build LibAtrac9
pushd vendor
git clone https://github.com/Thealexbarney/LibAtrac9.git --depth 1
cd LibAtrac9/C && make
mkdir -p ../include/libatrac9
mkdir -p ../libs
cp bin/libatrac9.a ../libs/
cp src/libatrac9.h ../include/libatrac9/
popd

# Build glm
# This step can be ommited if you have installed glm as a system dependency.
pushd vendor
git clone -b "0.9.9.3" https://github.com/g-truc/glm.git --depth 1
cd glm
cmake .
make
popd

# Executable expects the shaders dir to be nearby
cp -r src/shaders .

# Build impacto
LIBATRAC9DIR=vendor/LibAtrac9 cmake .
make
```
