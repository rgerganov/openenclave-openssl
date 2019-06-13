#include <openenclave/edger8r/enclave.h>
#include <openenclave/enclave.h>
#include "tests.h"
#include "openssl_t.h"
#include <sys/mount.h>

extern const char* __test__;

extern int main(int argc, const char* argv[]);

int enc_test(char *cwd, char test_name[STRLEN])
{
    static const char* argv[] = {
        "test",
        NULL,
    };
    static const int argc = sizeof(argv) / sizeof(argv[0]);

     if (OE_OK != oe_load_module_host_socket_interface())
    {        
        return(-1);
    }
    if (OE_OK != oe_load_module_host_resolver())
    {
        return(-1);
    }
    if (OE_OK != oe_load_module_host_file_system())
    {
        return(-1);
    }
    if (mount(cwd, "/", "oe_host_file_system", MS_RDONLY, NULL))
    {
        return(-1);
    }

    return main(argc, argv);
}

OE_SET_ENCLAVE_SGX(
    1,     /* ProductID */
    1,     /* SecurityVersion */
    true,  /* AllowDebug */
    12288, /* HeapPageCount */
    1024,  /* StackPageCount */
    8);    /* TCSCount */
