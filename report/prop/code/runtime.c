/* wrapper.c */
#include <sgx-lib.h>

int main(int argc, char **argv)
{
  sgx_init();
  /*
   * Take section information from ELF file as input
   * and compute entry_offset, program_size (code + data).
   */
  ...
  char *base = OpenSGX_loader("hello.sgx",
                              binary_size,
                              section_offset,
                              program_size);
  tcs_t *tcs = init_enclave(base,
                            entry,
                            program_size,
                            "hello.conf");
  sgx_enter(tcs, exception_handler);

  char *buf = malloc(11);
  sgx_host_read(buf, 11);
  printf("%s", buf);

  return 0;
}
