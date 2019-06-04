#include <openenclave/edger8r/enclave.h>
#include <openenclave/enclave.h>
#include "tests.h"
#include "openssl_t.h"
#include <sys/mount.h>

extern const char* __test__;

extern int main(int argc, const char* argv[]);

int enc_test(char test_name[STRLEN])
{
    static const char* argv[] = {
        "test",
        NULL,
    };
    static const int argc = sizeof(argv) / sizeof(argv[0]);

    oe_enable_feature(OE_FEATURE_HOST_FILES);
    oe_enable_feature(OE_FEATURE_HOST_SOCKETS);
    oe_enable_feature(OE_FEATURE_HOST_RESOLVER);
    oe_enable_feature(OE_FEATURE_POLLING);
    mount(".", "/", "hostfs", MS_RDONLY, NULL);

    return main(argc, argv);
}

OE_SET_ENCLAVE_SGX(
    1,     /* ProductID */
    1,     /* SecurityVersion */
    true,  /* AllowDebug */
    12288, /* HeapPageCount */
    1024,  /* StackPageCount */
    8);    /* TCSCount */
