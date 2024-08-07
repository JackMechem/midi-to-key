cmake_minimum_required(VERSION 3.29)
project(midi-to-key VERSION 1.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

find_package(RtMidi)
# add_subdirectory(lib/rtmidi)

include(FetchContent)
FetchContent_Declare(
  tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG v3.4.0)
FetchContent_MakeAvailable(tomlplusplus)

add_executable(midi-to-key src/main.cpp)

target_include_directories(midi-to-key PUBLIC src)

target_link_libraries(midi-to-key tomlplusplus)
target_link_libraries(midi-to-key RtMidi::rtmidi)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  install(TARGETS ${PROJECT_NAME} DESTINATION bin)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
