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
## Build and install openssl for open enclave

```bash
cd ~/openenclave-openssl/openssl
git am  ../0001-Get-openssl-to-build-against-MUSL-headers.-Use-SGX-r.patch
./config --with-rand-seed=none no-idea no-mdc2 no-rc5 no-rc4 no-bf no-ec2m no-camellia no-cast no-srp no-hw no-dso no-shared no-ssl3 no-md2 no-md4 no-afalgeng -D_FORTIFY_SOURCE=2 -DGETPID_IS_MEANINGLESS --prefix=/opt/oe-openssl CC=/opt/musl/bin/musl-gcc
make all install
```

## Build and install the right branch of openenclave and a patch
```bash
git clone https://github.com/microsoft/openenclave.git oe_patched
cd oe_patched
git am ~/openenclave-openssl/0001-Add-support-for-recursive-calls-to-oe_once-pthread_o.patch #Apply a patch to allow a recursive lock
mkdir build
cd build
cmake ..
make
cmake -DCMAKE_INSTALL_PREFIX=/opt/openenclave ..
sudo make install
. /opt/openenclave/share/openenclave/openenclaverc
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

