# Prototype for using the OpenSSL libraries with the Open Enclave SDK
## Build and install the same version of MUSL used in Open Enclave
```bash
wget http://www.musl-libc.org/releases/ musl-1.1.21.tar.gz

tar zxvf musl-1.1.21.tar.gz

cd musl-1.1.21

./configure CFLAGS=-fPIC --prefix=/opt/musl --disable-shared

make

make install
```
## Clone the repo
```bash
git clone https://github.com/openenclave/openenclave-openssl --recursive
```

## Build and install openssl for open enclave

```bash

cd ~/openenclave-openssl/openssl

git am  ../0001-Get-openssl-to-build-against-MUSL-headers.-Use-SGX-r.patch

./config --with-rand-seed=none no-idea no-mdc2 no-rc5 no-rc4 no-bf no-ec2m no-camellia no-cast no-srp no-hw no-dso no-shared no-ssl3 no-md2 no-md4 no-afalgeng -D_FORTIFY_SOURCE=2 -DGETPID_IS_MEANINGLESS --prefix=/opt/oe-openssl CC=/opt/musl/bin/musl-gcc

make all install
```


## Build the sample
```bash
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/opt/oe-openssl/lib/pkgconfig
cd ~/openenclave-openssl/sample
make
```

## Run the sample which is a TLS server running inside an enclave
```bash
host/openssl_serverhost enc/openssl_server_enc.signed
```

## Connect to the server running inside an enclave. Run the following from a different terminal
```bash
openssl s_client -connect localhost:4433
```

# Contributing

This project welcomes contributions and suggestions. Most contributions require you to
agree to a Contributor License Agreement (CLA) declaring that you have the right to,
and actually do, grant us the rights to use your contribution. For details, visit
https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need
to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the
instructions provided by the bot. You will only need to do this once across all repositories using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
