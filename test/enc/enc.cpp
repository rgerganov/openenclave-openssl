#include <openenclave/edger8r/enclave.h>
#include <openenclave/enclave.h>
#include "tests.h"
#include <pthread.h>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <map>
#include <thread>
#include <vector>
#include "openssl_t.h"

extern const char* __test__;

extern "C" int main(int argc, const char* argv[]);

int enc_test(char test_name[STRLEN])
{
    static const char* argv[] = {
        "test",
        NULL,
    };
    static const int argc = sizeof(argv) / sizeof(argv[0]);

    return main(argc, argv);
}

OE_SET_ENCLAVE_SGX(
    1,     /* ProductID */
    1,     /* SecurityVersion */
    true,  /* AllowDebug */
    12288, /* HeapPageCount */
    1024,  /* StackPageCount */
    8);    /* TCSCount */
