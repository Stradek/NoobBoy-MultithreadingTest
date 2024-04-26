cmake_minimum_required(VERSION 3.5)
project (NoobBoy)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

set(EXTERNAL_LIBS_DIR "${PROJECT_SOURCE_DIR}/external/")

find_package(SDL2 REQUIRED PATHS "${EXTERNAL_LIBS_DIR}")
find_package(SDL2_ttf REQUIRED PATHS "${EXTERNAL_LIBS_DIR}")
find_package(SDL2_mixer REQUIRED PATHS "${EXTERNAL_LIBS_DIR}")

include_directories("/usr/local/include")
include_directories("/opt/homebrew/include")
include_directories("/opt/homebrew/lib")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set (SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/")

include_directories(${SOURCE_DIR})
link_directories(${SOURCE_DIR})

file (GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE "${EXTERNAL_LIBS_DIR}/include")
target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::TTF SDL2::Mixer)
