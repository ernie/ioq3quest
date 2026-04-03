# Building libopenal.so for Android (arm64-v8a)

The pre-built `libopenal.so` in this directory was built from OpenAL-Soft in Release mode
with OpenSL backend support for Android audio.

## Prerequisites

- Android NDK (tested with 27.3.13750724)
- CMake 3.10+
- Ninja (optional, but recommended)

## Source

**OpenAL-Soft**: https://github.com/kcat/openal-soft

Clone with:
```
git clone https://github.com/kcat/openal-soft.git
```

## Build Script (Windows, Git Bash)

```bash
ANDROID_SDK_ROOT="$LOCALAPPDATA/Android/Sdk"
NDK_VERSION=27.3.13750724
NDK_PATH="$ANDROID_SDK_ROOT/ndk/$NDK_VERSION"
TOOLCHAIN_FILE="$NDK_PATH/build/cmake/android.toolchain.cmake"
STRIP="$NDK_PATH/toolchains/llvm/prebuilt/windows-x86_64/bin/llvm-strip.exe"

cmake -B /tmp/openal-build -S /tmp/openal-soft \
  -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release \
  -DALSOFT_UTILS=OFF \
  -DALSOFT_EXAMPLES=OFF \
  -DALSOFT_TESTS=OFF \
  -DALSOFT_INSTALL=OFF \
  -DALSOFT_BACKEND_OPENSL=ON \
  -DALSOFT_BACKEND_OBOE=ON \
  -G "Ninja"

cmake --build /tmp/openal-build -j $NUMBER_OF_PROCESSORS

"$STRIP" -o libopenal.so /tmp/openal-build/libopenal.so
```

## Output

After building, copy the stripped `libopenal.so` to this directory.
Expected size is ~2MB for a stripped Release build.

## License

**OpenAL-Soft**: GNU Library General Public License, version 2 (LGPL-2)
https://github.com/kcat/openal-soft/blob/master/COPYING

OpenAL-Soft is dynamically linked (`libopenal.so` loaded via `dlopen` at runtime),
which satisfies LGPL requirements for use in non-LGPL applications. Users may
replace the bundled `libopenal.so` with their own build. The complete source code
is available at the repository linked above.
