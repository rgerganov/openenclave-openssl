# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/enc")
include(get_testcase_name)
add_custom_command(OUTPUT openssl_u.h openssl_u.c openssl_args.h
DEPENDS ${CMAKE_SOURCE_DIR}/openssl.edl
COMMAND openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/openssl.edl)


add_executable(openssl_host host.cpp openssl_u.c openssl_args.h)

target_link_libraries(openssl_host openenclave::oehostapp)

target_include_directories(openssl_host PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
