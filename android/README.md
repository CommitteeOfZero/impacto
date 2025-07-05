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

# Copy Assets
Copy profiles, shaders, games into impacto/android/app/src/main/assets

run ./gradlew assemble in impacto/android
apks will be created in impacto/android/distribution/android/app/outputs/apk

# Debugging
Override the CMake preset and set CMAKE_BUILD_TYPE to DEBUG (-DCMAKE_BUILD_TYPE=DEBUG in command line or override with user preset) 
to build with debug symbols first.
In Android Studio, open the impacto/android folder
Make sure the explorer is set to view Project or Project files and not Android so cpp files are actually visible.
Edit the run configuratio
- Go to debugger:
- Add <absolute_path_to_impacto>/android/app/src/main/cpp/impacto to symbol directories
- In LLDB Startup Commands, add the following so symbols get mapped to correct files
`settings append target.source-map <absolute_path_to_impacto> <absolute_path_to_impacto>/android/app/src/main/cpp/impacto`
Breakpoints should work now properly in C++ files when debugging