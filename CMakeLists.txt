cmake_minimum_required(VERSION 3.16)
project(PhongThan2Recovery LANGUAGES C)

# Partial, isolated build. Does not compile or link the Windows game.
include(CTest)
add_subdirectory(Base/LuaLib)
add_subdirectory(Base/Common)
