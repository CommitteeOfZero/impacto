Android Development:

# Set up Android NDK
Download the Android NDK
Set $ANDROID_NDK_HOME to point to NDK path
Set $MINSDKVERSION to point to desired minimum Android API level (currently 28 in pipeline)

# Build impacto
Run cmake with the ci-release-android preset or customize with your own in CMakeUserPresets.json
VCPKG will automatically build dependencies for Android ARM64 target defined in the custom triplet.

```shell
cmake --preset ci-release-android
cmake --build --preset ci-release-android
```
libimpacto.so will automatically be copied to impacto/android/app/src/main/jniLibs/<ABI>/libimpacto.so

# Packaging
To avoid issues with packaging the .apk, avoid bundling game assets as .apk files have a maximum file size limit.
run ./gradlew assemble in impacto/android
apks will be created in impacto/android/distribution/android/app/outputs/apk

# Preparing impacto
Game assets can be copied to the `/sdcard/Android/data/com.committeeofzero.impacto/files/games` folder, or a user selectable directory. 
If choosing a custom directory, a prompt to enable the manage all files permission will be given and it will need to be enabled. 
See [Getting Started](/doc/getting_started.md) for more information about files needed.
Upon running the application for the first time, bundled files will be copied to the `/sdcard/Android/data/com.committeeofzero.impacto/files/` folder.

# Debugging
Override the CMake preset and set CMAKE_BUILD_TYPE to DEBUG (-DCMAKE_BUILD_TYPE=DEBUG in command line or override with user preset) 
to build with debug symbols first.
In Android Studio, open the impacto/android folder
Make sure the explorer is set to view Project or Project files and not Android so cpp files are actually visible.
Run in debug configuration.
Set breakpoints in files located in the app/src/main/cpp/impacto symlink.