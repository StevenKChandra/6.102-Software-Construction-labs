cmake_minimum_required(VERSION 3.28.0)

set(CXX_STANDARD C++ 17)
set(CXX_STANDARD_REQUIRED true)

project(MultiplayerMinesweeper VERSION 1.0 LANGUAGES CXX)

set(OPENSSL_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl-src)
set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_INSTALL_DIR}/include)
set(OPENSSL_CONFIGURE_COMMAND ${OPENSSL_SOURCE_DIR}/config)

include(ExternalProject)
ExternalProject_Add(
  OpenSSL
  SOURCE_DIR ${OPENSSL_SOURCE_DIR}
  URL https://github.com/openssl/openssl/releases/download/openssl-3.4.1/openssl-3.4.1.tar.gz
  URL_HASH SHA256=002a2d6b30b58bf4bea46c43bdd96365aaf8daa6c428782aa4feee06da197df3
  USES_TERMINAL_DOWNLOAD TRUE
  CONFIGURE_COMMAND
    ${OPENSSL_CONFIGURE_COMMAND}
    --prefix=${OPENSSL_INSTALL_DIR}
    --openssldir=${OPENSSL_INSTALL_DIR}
  BUILD_COMMAND make
  TEST_COMMAND ""
  INSTALL_COMMAND make install
  INSTALL_DIR ${OPENSSL_INSTALL_DIR}
)

file(MAKE_DIRECTORY ${OPENSSL_INCLUDE_DIR})

add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib64/libssl.so)
set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
add_dependencies(OpenSSL::SSL OpenSSL)

add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib64/libcrypto.so)
set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
add_dependencies(OpenSSL::Crypto OpenSSL)

set(
    SRC_FILES
    src/board_implementation.cpp
    src/minesweeper_server.cpp
)

set(CMAKE_BUILD_TYPE Release)
add_executable(MultiplayerMinesweeper "${SRC_FILES}")

target_include_directories(
    MultiplayerMinesweeper
    PUBLIC include
)

target_link_libraries(
    MultiplayerMinesweeper
    OpenSSL::SSL
    OpenSSL::Crypto
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip 
)
FetchContent_MakeAvailable(googletest)

enable_testing()

set (
    TEST_FILES
    test/board_implementation_test.cpp
)

set(CMAKE_BUILD_TYPE Debug)
add_executable(MultiplayerMinesweeperTest "${TEST_FILES}" "${SRC_FILES}")

target_include_directories(
    MultiplayerMinesweeperTest
    PRIVATE include
)

target_link_libraries(
    MultiplayerMinesweeperTest
    GTest::gtest_main
    OpenSSL::SSL
    OpenSSL::Crypto
)

include(GoogleTest)

gtest_discover_tests(MultiplayerMinesweeperTest)