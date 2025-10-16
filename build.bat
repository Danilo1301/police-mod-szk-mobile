@echo off
set NDK=C:\android-ndk-r26d

:: 🧱 Build 32 bits (armeabi-v7a)
echo Building armeabi-v7a...
cmake -S . -B build/armeabi-v7a -G Ninja ^
  -DCMAKE_TOOLCHAIN_FILE=%NDK%\build\cmake\android.toolchain.cmake ^
  -DANDROID_ABI=armeabi-v7a ^
  -DANDROID_PLATFORM=android-21
cmake --build build/armeabi-v7a --config Release

:: pause