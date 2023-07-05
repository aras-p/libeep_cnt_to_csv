cmake_minimum_required(VERSION 3.8)

set(LIBEEP_VERSION_MAJOR 3)
set(LIBEEP_VERSION_MINOR 3)
set(LIBEEP_VERSION_PATCH 178)
set(LIBEEP_VERSION "${LIBEEP_VERSION_MAJOR}.${LIBEEP_VERSION_MINOR}.${LIBEEP_VERSION_PATCH}")

project(libeep_cnt_to_csv)

add_definitions(-DLIBEEP_VERSION_MAJOR=${LIBEEP_VERSION_MAJOR})
add_definitions(-DLIBEEP_VERSION_MINOR=${LIBEEP_VERSION_MINOR})
add_definitions(-DLIBEEP_VERSION_PATCH=${LIBEEP_VERSION_PATCH})
if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DNOMINMAX)
endif(WIN32)

set(Eep_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libavr/avr.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libavr/avrcfg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/cnt.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/cntutils.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/evt.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/raw3.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/rej.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/riff64.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/riff.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/seg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libcnt/trg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/eepio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/eepmem.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/eepmisc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/eepraw.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/val.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/libeep/var_string.c
  ${CMAKE_CURRENT_SOURCE_DIR}/libeep-3.3.177/src/v4/eep.c
)

include_directories(libeep-3.3.177/src)

add_executable(libeep_cnt_to_csv
  main.c
  ${Eep_sources}
)
