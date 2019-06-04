cmake_minimum_required(VERSION 3.11)

project("OpenEnclave for OpenSSL Tests" LANGUAGES C CXX)

find_package(OpenEnclave CONFIG REQUIRED)

enable_testing()

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
    get_testcase_name(${testcase} name "../../openssl/test/")
    add_enclave_test(${name}_test host/openssl_host enc/openssl-${name}_enc)
endforeach(testcase)
