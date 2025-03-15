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
