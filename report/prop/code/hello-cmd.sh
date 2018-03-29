$ opensgx -k
> generate sign.key
$ opensgx -c hello.c
> generate hello.sgx executable
$ opensgx -s hello.sgx --key sign.key
> generate hello.conf
$ opensgx hello.sgx hello.conf
hello sgx!
