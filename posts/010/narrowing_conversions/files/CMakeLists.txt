cmake_minimum_required(VERSION 3.21...3.22)

project(cxx20_samples 
    LANGUAGES C CXX)

## Put all binary files into /bin and libraries into /lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/bin)

## Add source code
add_executable(narrowing_conversions ${CMAKE_CURRENT_SOURCE_DIR}/narrowing_conversions_20.cpp)
target_compile_features(narrowing_conversions PRIVATE cxx_std_20)
