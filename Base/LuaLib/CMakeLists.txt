cmake_minimum_required(VERSION 3.16)
project(PhongThan2LegacyLua LANGUAGES C)

# Portable static build of the bundled legacy Lua sources.
# Keeps VC6 projects untouched. Do not include src/lua.c (standalone CLI).
add_library(pt_legacy_lua STATIC
    src/lapi.c
    src/lcode.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/llex.c
    src/lmem.c
    src/lobject.c
    src/lopcodes.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/ltable.c
    src/ltests.c
    src/ltm.c
    src/lundump.c
    src/lvm.c
    src/lzio.c
    src/baselib/lauxlib.c
    src/baselib/lbaselib.c
    src/baselib/ldblib.c
    src/baselib/liolib.c
    src/baselib/lmathlib.c
    src/baselib/loadlib.c
    src/baselib/lstrlib.c
    src/baselib/ltablib.c
)

target_include_directories(pt_legacy_lua PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set_target_properties(pt_legacy_lua PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
