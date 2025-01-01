# rlottie 库:

cmake_minimum_required(VERSION 3.14)

set(BUILD_SHARED_LIBS OFF)
set(LOTTIE_MODULE OFF)
set(LOTTIE_THREAD OFF)
set(LOTTIE_TEST OFF)

set(CMAKE_MESSAGE_LOG_LEVEL NONE)
# add_library(rlottie STATIC)
# add_subdirectory(lib_rlottie/inc)
# add_subdirectory(lib_rlottie/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_rlottie)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W")
set(CMAKE_MESSAGE_LOG_LEVEL WARNING)

target_include_directories(app_thread PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib_rlottie/inc")
target_link_libraries(app_thread PUBLIC rlottie)
