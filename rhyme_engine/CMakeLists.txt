file(
  GLOB
  rhyme_engine_src
  *.cpp
  *.h
)

add_library(
  rhyme_engine
  STATIC
  ${rhyme_engine_src}
)

target_include_directories(
  rhyme_engine
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
