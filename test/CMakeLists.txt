cmake_minimum_required(VERSION 3.11)

project("OpenEnclave for OpenSSL Tests" LANGUAGES C CXX)

find_package(OpenEnclave CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/enc")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
message(STATUS "RADHIKAJ CMAKE_MODULE_PATH"  ${CMAKE_MODULE_PATH})
include(get_testcase_name)
include(add_enclave_test)

if (ENABLE_FULL_OPENSSL)
    file(STRINGS "tests.supported" alltests)
else()
    file(STRINGS "tests.supported.default" alltests)
endif()

add_subdirectory(enc)
add_subdirectory(host)


foreach(testcase ${alltests})
    message("test here")
    get_testcase_name(${testcase} name "../../openssl/test/")
    message("test here2")
    add_enclave_test(host/openssl_test enc/test-${name}_enc)
endforeach(testcase)
