# Supported test-build alternative: JUCE 7.0.12, MinGW-w64 POSIX x64.
# Configure with a host JUCE package and -DBM_JUCE_VERSION=7.0.12.
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
find_program(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-posix REQUIRED)
find_program(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix REQUIRED)
find_program(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres REQUIRED)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")
