# /CMakeLists.txt

cmake_minimum_required(VERSION 3.21)
project(BlurhashCXX LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

#Prepare blurhash-ref
set(BLURHASH_REF_DIR "${CMAKE_SOURCE_DIR}/blurhash-ref/C")
add_library(BlurhashREF STATIC ${BLURHASH_REF_DIR}/decode.c ${BLURHASH_REF_DIR}/encode.c)
target_include_directories(BlurhashREF PUBLIC ${CMAKE_SOURCE_DIR}/blurhash-ref/C)

add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/gtest)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/gbench)

#Prepare tests
file(GLOB BLURHASH_TESTS ${CMAKE_SOURCE_DIR}/tests/**.cpp)
add_executable(BlurhashTESTS ${BLURHASH_TESTS})
target_link_libraries(BlurhashTESTS PRIVATE BlurhashREF GTest::gtest_main)
target_include_directories(BlurhashTESTS PUBLIC ${CMAKE_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(BlurhashTESTS)

#Prepare tests
file(GLOB BLURHASH_BENCHES ${CMAKE_SOURCE_DIR}/bench/**.cpp)
add_executable(BlurhashBENCH ${BLURHASH_BENCHES})
target_link_libraries(BlurhashBENCH PRIVATE BlurhashREF benchmark::benchmark_main)
target_include_directories(BlurhashBENCH PUBLIC ${CMAKE_SOURCE_DIR}/include)
