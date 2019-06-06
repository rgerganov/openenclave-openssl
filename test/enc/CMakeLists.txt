list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/enc")
include(get_testcase_name)
add_custom_command(OUTPUT openssl_t.h openssl_t.c openssl_args.h
DEPENDS ${CMAKE_SOURCE_DIR}/openssl.edl
COMMAND openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/openssl.edl)
  
function(add_enclave)
   set(options CXX)
   set(oneValueArgs TARGET CONFIG KEY)
   set(multiValueArgs SOURCES)
   cmake_parse_arguments(ENCLAVE
     "${options}"
     "${oneValueArgs}"
     "${multiValueArgs}"
     ${ARGN})

   add_executable(${ENCLAVE_TARGET} ${ENCLAVE_SOURCES})
   target_link_libraries(${ENCLAVE_TARGET}  INTERFACE openenclave:oeenclave)
   if (ENCLAVE_CXX)
     target_link_libraries(${ENCLAVE_TARGET} INTERFACE openenclave:oelibcxx)
   endif ()

  # Cross-compile if needed.
  if (USE_CLANGW)
    maybe_build_using_clangw(${ENCLAVE_TARGET})

    # maybe_build_using_clangw populates variables in its parent scope (ie current scope)
    # Propagate these variables back up to the caller.

    # Propagate library names variables
    set(CMAKE_STATIC_LIBRARY_PREFIX "${CMAKE_STATIC_LIBRARY_PREFIX}" PARENT_SCOPE)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "${CMAKE_STATIC_LIBRARY_SUFFIX}" PARENT_SCOPE)

    # Propagate library tool variables
    set(CMAKE_C_CREATE_STATIC_LIBRARY "${CMAKE_C_CREATE_STATIC_LIBRARY}" PARENT_SCOPE)
    set(CMAKE_CXX_CREATE_STATIC_LIBRARY "${CMAKE_CXX_CREATE_STATIC_LIBRARY}" PARENT_SCOPE)

    # Propagate linker variables
    set(CMAKE_EXECUTABLE_SUFFIX "${CMAKE_EXECUTABLE_SUFFIX}" PARENT_SCOPE)
    set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}" PARENT_SCOPE)
    set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}" PARENT_SCOPE)
    set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES}" PARENT_SCOPE)
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}" PARENT_SCOPE)

    # Propagate cpmpiler variables
    set(CMAKE_C_COMPILE_OBJECT "${CMAKE_C_COMPILE_OBJECT}" PARENT_SCOPE)
    set(CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT}" PARENT_SCOPE)
  endif()

   if (NOT ENCLAVE_CONFIG)
      # Since the config is not specified, the enclave wont be signed.
      return()
   endif ()

  # Generate the signing key.
  if(NOT ENCLAVE_KEY)
     add_custom_command(OUTPUT ${ENCLAVE_TARGET}-private.pem
       COMMAND openssl genrsa -out ${ENCLAVE_TARGET}-private.pem -3 3072)
     set(ENCLAVE_KEY  ${CMAKE_CURRENT_BINARY_DIR}/${ENCLAVE_TARGET}-private.pem)
  endif()

  # TODO: Get this name intelligently (somehow use $<TARGET_FILE> with
  # `.signed` injected).
  set(SIGNED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${ENCLAVE_TARGET}.signed)

  # Sign the enclave using `oesign`.
  if(ENCLAVE_CONFIG)
    add_custom_command(OUTPUT ${SIGNED_LOCATION}
      COMMAND oesign sign -e $<TARGET_FILE:${ENCLAVE_TARGET}> -c ${ENCLAVE_CONFIG} -k ${ENCLAVE_KEY}
      DEPENDS oesign ${ENCLAVE_TARGET} ${ENCLAVE_CONFIG} ${ENCLAVE_KEY}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()

  # Import the generated signed enclave so we can reference it with
  # `$<TARGET_FILE>` later.
  add_library(${ENCLAVE_TARGET}_signed SHARED IMPORTED GLOBAL)
  set_target_properties(${ENCLAVE_TARGET}_signed PROPERTIES
    IMPORTED_LOCATION ${SIGNED_LOCATION})

  # Add a custom target with `ALL` semantics so these targets are always built.
  add_custom_target(${ENCLAVE_TARGET}_signed_target ALL DEPENDS ${SIGNED_LOCATION})
endfunction(add_enclave)


# helper lib to contain file needed by some tests
add_library(openssl-support
   ../../openssl/test/testutil/basic_output.c 
   ../../openssl/test/testutil/cb.c
   ../../openssl/test/testutil/driver.c
   ../../openssl/test/testutil/format_output.c 
   ../../openssl/test/testutil/init.c 
   ../../openssl/test/testutil/main.c
   ../../openssl/test/testutil/output_helpers.c
   ../../openssl/test/testutil/stanza.c
   ../../openssl/test/testutil/tap_bio.c
   ../../openssl/test/testutil/test_cleanup.c
   ../../openssl/test/testutil/tests.c
   ../../openssl/test/ssltestlib.c
   ../../openssl/test/ssl_test_ctx.c
   ${TESTFILE}
    )


target_compile_options(openssl-support PRIVATE
    -Wno-error=attributes
    -Wno-error=strict-overflow
    -Wno-error=unused-local-typedefs
    -Wno-error=unused-variable
    -Wno-sign-conversion
    )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(openssl-support PRIVATE
        -Wno-error=maybe-uninitialized
        -Wno-error=unused-but-set-variable
        )
endif ()

target_link_libraries(openssl-support PUBLIC /opt/openenclave-featureio2/lib/openenclave/enclave/liboecpio.a /opt/openenclave-featureio2/lib/openenclave/enclave/liboesgxfsenclave.a  openenclave::oeenclave openenclave::oelibc)
target_link_libraries(openssl-support INTERFACE -Wl,--undefined=Test)

target_include_directories(openssl-support PRIVATE ../../openssl/include/openssl ../../openssl/include ../../openssl/include/internal ../../openssl/test ../../openssl ../../openssl/crypto/include ../../openssl/crypto/ec)

add_library(openssl_imported IMPORTED STATIC)
    set_target_properties(openssl_imported PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../openssl/libssl.a)
    add_library(openssl_crypto_imported IMPORTED STATIC)
    set_target_properties(openssl_crypto_imported PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../openssl/libcrypto.a)

function(add_openssl_test_enc NAME TESTFILE)
    add_enclave(TARGET openssl-${NAME}_enc SOURCES
        ${TESTFILE} enc.c ${CMAKE_CURRENT_BINARY_DIR}/openssl_t.c )
        
    target_include_directories(openssl-${NAME}_enc PRIVATE ../../openssl/include/openssl ../../openssl/include ../../openssl/include/internal ../../openssl/test ../../openssl ../../openssl/crypto/include ../../openssl/crypto/ec/curve448/ ../../openssl/crypto/ec)

      target_compile_options(openssl-${NAME}_enc PRIVATE
        # These are third-party tests, so we don't care about their warnings.
        -Wno-error
        -Wno-unused-function
        -Wno-unused-local-typedef
        -Wno-deprecated-declarations
        # Remove NDEBUG to enable the libcxx testsuite assertions in Release
        -UNDEBUG
        )

    target_include_directories(openssl-${NAME}_enc PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    
    target_link_libraries(openssl-${NAME}_enc PUBLIC openssl-support)
    target_link_libraries(openssl-${NAME}_enc PUBLIC openssl_imported openssl_crypto_imported)    
    target_link_libraries(openssl-${NAME}_enc PUBLIC  /opt/openenclave-featureio2/lib/openenclave/enclave/liboecpio.a /opt/openenclave-featureio2/lib/openenclave/enclave/liboesgxfsenclave.a   openenclave::oeenclave openenclave::oelibcxx)
    
endfunction(add_openssl_test_enc)

if (ENABLE_FULL_LIBCXX_TESTS)
    file(STRINGS "../tests.supported" alltests)
else()
    file(STRINGS "../tests.supported.default" alltests)
endif()
foreach(testcase ${alltests})     
    get_testcase_name(${testcase} name "../../openssl/test/")
    add_openssl_test_enc("${name}" "${testcase}")
endforeach(testcase)
