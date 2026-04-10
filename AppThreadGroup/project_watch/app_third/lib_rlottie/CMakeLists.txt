# rlottie 库:
set(CMAKE_MESSAGE_LOG_LEVEL NONE)

cmake_minimum_required(VERSION 3.14)

set(BUILD_SHARED_LIBS OFF)
set(LOTTIE_MODULE OFF)
set(LOTTIE_THREAD OFF)
set(LOTTIE_TEST OFF)

# add_library(rlottie STATIC)
# add_subdirectory(lib_rlottie/inc)
# add_subdirectory(lib_rlottie/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib_rlottie)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W")

target_include_directories(app_thread PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib_rlottie/inc")
target_link_libraries(app_thread PRIVATE rlottie)

# 豆包:给thorvg设置动态运行时库（MD/MDd）
if(MSVC)
    target_compile_options(rlottie PRIVATE
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
    )
endif()
if(MSVC)
    target_compile_options(lottie2gif PRIVATE
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
    )
endif()

set(CMAKE_MESSAGE_LOG_LEVEL WARNING)
