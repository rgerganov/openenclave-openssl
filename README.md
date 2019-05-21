# For using the OpenSSL libraries with the Open Enclave SDK
## Clone the repo
```bash
git clone https://github.com/openenclave/openenclave-openssl
```

## Build and install the same version of MUSL used in Open Enclave
```bash
wget http://www.musl-libc.org/releases/ musl-1.1.21.tar.gz

tar zxvf musl-1.1.21.tar.gz

cd musl-1.1.21

./configure CFLAGS=-fPIC --prefix=/opt/musl --disable-shared

make

make install
```
## Build and install the same version of MUSL used in Open Enclave

```bash
cd ~/openenclave-openssl/openssl
git am  ../0001-Get-openssl-to-build-against-MUSL-headers.-Use-SGX-r.patch
./config --with-rand-seed=none no-idea no-mdc2 no-rc5 no-rc4 no-bf no-ec2m no-camellia no-cast no-srp no-hw no-dso no-shared no-ssl3 no-md2 no-md4 no-afalgeng -D_FORTIFY_SOURCE=2 -DGETPID_IS_MEANINGLESS --prefix=/opt/oe-openssl CC=/opt/musl/bin/musl-gcc
make all install
```
