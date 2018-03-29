/* hello.c */
#include <sgx-lib.h>

void enclave_main()
{
    char *hello = "hello sgx!\n";
    sgx_enclave_write(hello, sgx_strlen(hello));
    sgx_exit(NULL);
}
