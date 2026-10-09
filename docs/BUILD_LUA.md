# Experimental Lua library compilation

The bundled Lua C sources are a first isolated build milestone. **They do not prove** that the Windows client or server can be built or run.

## Build-only verification

Run from repository root:

```sh
cmake -S Base/LuaLib -B build/legacy-lua -DCMAKE_BUILD_TYPE=Release
cmake --build build/legacy-lua --parallel
```

The result on Unix is `build/legacy-lua/libpt_legacy_lua.a`. CI compiles a library **without executing game binaries** and does not create a game release.

## Compatibility and safety

- The game uses Visual C++ 6.0 Win32 x86 ABI and precompiled proprietary `.lib` dependencies. A Linux GCC archive **cannot** be linked into the original Windows client or server.
- The legacy Lua source has pointer-to-integer casts and may emit warnings on 64-bit systems. Compilation does not establish correctness or runtime safety.
- The original `Base/LuaLib/LuaLib.dsp` remains the Windows project. This independent CMake target omits `src/lua.c`, the standalone Lua CLI.
- A VC6-only reference to the absent `luadebug.h` was removed from the project manifest; actual `ldebug.h` remains included. No replacement header has been invented.
- The next milestone is a controlled Windows x86 build and assessment of linker errors. Keep legacy binaries isolated.

Run `python -m unittest discover -s tests -v` to validate manifest consistency.
