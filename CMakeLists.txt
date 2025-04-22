cmake_minimum_required(VERSION 3.16)
project(byte_pair_encoding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BYTE_PAIR_ENCODE_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/byte_pair_encoding.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/byte_pair_encoding.h
)

add_executable(byte_pair_encode)
target_sources(byte_pair_encode
 PRIVATE
  ${BYTE_PAIR_ENCODE_SOURCES}
)
target_include_directories(byte_pair_encode
 PRIVATE
  include
)

set(VEC_SWAP_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vec_swap_test.c
)
add_executable(vec_swap_test)
target_sources(vec_swap_test
 PRIVATE
  ${VEC_SWAP_TEST_SOURCES}
)
target_include_directories(vec_swap_test
 PRIVATE
  include
)

set(PAIRS2DOT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pairs2dot.c
  ${CMAKE_CURRENT_SOURCE_DIR}/include/byte_pair_encoding.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/byte_pair_encoding.c
)
add_executable(pairs2dot)
target_sources(pairs2dot
 PRIVATE
  ${PAIRS2DOT_SOURCES}
)
target_include_directories(pairs2dot
 PRIVATE
  include
)
