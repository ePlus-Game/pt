# Portable subset of Common. The full Common.dsp is Windows/VC6-only.
# GCC considers uppercase .C a C++ suffix; copy to .c in the build directory
# so the archived original remains byte-for-byte intact.
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/CRC32.C" "${CMAKE_CURRENT_BINARY_DIR}/crc32_portable.c" COPYONLY)
add_library(pt_common_portable STATIC "${CMAKE_CURRENT_BINARY_DIR}/crc32_portable.c" minilzo.c)
target_compile_definitions(pt_common_portable PRIVATE _STANDALONE)
target_include_directories(pt_common_portable PRIVATE "${CMAKE_SOURCE_DIR}/Share/Header")
set_target_properties(pt_common_portable PROPERTIES C_STANDARD 90 C_STANDARD_REQUIRED ON)

if(BUILD_TESTING)
  add_executable(pt_test_crc32 tests/crc32_test.c)
  target_link_libraries(pt_test_crc32 PRIVATE pt_common_portable)
  add_test(NAME legacy_crc32_known_vector COMMAND pt_test_crc32)
endif()
