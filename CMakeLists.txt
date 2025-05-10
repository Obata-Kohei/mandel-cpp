cmake_minimum_required(VERSION 3.16)
project(mandel CXX)

# C++17を有効にする
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 最適化と警告
add_compile_options(-Wall -Wextra -O2)

# ディレクトリ設定
set(SRC_DIR src)
set(INCLUDE_DIR include)
set(TEST_DIR test)

include_directories(${INCLUDE_DIR})

# ソースファイルを集める
file(GLOB SRC_FILES ${SRC_DIR}/*.cpp)

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found")
    target_compile_options(OpenMP::OpenMP_CXX INTERFACE ${OpenMP_CXX_FLAGS})
else()
    message(WARNING "OpenMP not found")
endif()

# Boost (system, filesystem)
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# libpng
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

# GMP / MPFR / MPC（Boost.Multiprecisionが依存する可能性あり）
find_library(GMP_LIB gmp)
find_library(MPFR_LIB mpfr)
find_library(MPC_LIB mpc)

# 実行ファイル
add_executable(prg ${SRC_FILES})
target_link_libraries(prg
    ${Boost_LIBRARIES}
    ${PNG_LIBRARY}
    ${GMP_LIB}
    ${MPFR_LIB}
    ${MPC_LIB}
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(prg OpenMP::OpenMP_CXX)
endif()