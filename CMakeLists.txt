cmake_minimum_required(VERSION 3.20)

project(Engine LANGUAGES CXX)

# =====================
# Fmt
# =====================

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1

FetchContent_MakeAvailable(fmt)

# =====================
# Options
# =====================
option(ENABLE_CUDA "Enable CUDA backend" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =====================
# Engine library
# =====================
add_library(Engine STATIC
        # Core
        src/Engine.cpp
        src/Pipeline.cpp
        src/Job.cpp
        src/Scheduler.cpp

        # IO
        src/io/DecoderFFmpeg.cpp
        src/io/EncoderFFmpeg.cpp

        # CPU backend
        src/backend/cpu/CpuBackend.cpp

        # Metadata
        src/metadata.rc

        # Utils
        src/utils/Logger.h
        src/utils/ThreadPool.h
        src/utils/Timer.h
)

target_include_directories(Engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# =====================
# CUDA backend (optional)
# =====================
if (ENABLE_CUDA)
    enable_language(CUDA)

    target_sources(Engine
            PRIVATE
            src/backend/cuda/CudaBackend.cu
    )

    set_target_properties(Engine PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
    )

    target_compile_definitions(Engine
            PUBLIC ENGINE_HAS_CUDA
    )
endif ()

# =====================
# Test
# =====================

add_executable(engine_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/engine_tests.cpp)

if(WIN32)
    target_link_options(engine_tests PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

target_include_directories(engine_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(engine_tests PRIVATE Engine comdlg32 fmt::fmt)

# ==========================================
# FFmpeg Manual Linking (Windows/MinGW)
# ==========================================

set(FFMPEG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/ffmpeg/include")
set(FFMPEG_LIB_DIR     "${CMAKE_SOURCE_DIR}/ffmpeg/lib")

target_include_directories(Engine PUBLIC ${FFMPEG_INCLUDE_DIR})

target_link_directories(Engine PUBLIC ${FFMPEG_LIB_DIR})

# Note: On Windows MinGW, they might be named 'avcodec.lib' or 'libavcodec.a'
target_link_libraries(Engine PUBLIC
        avcodec
        avformat
        avutil
        swscale
        fmt::fmt
)