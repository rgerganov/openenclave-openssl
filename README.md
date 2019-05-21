# openssl
For using the OpenSSL libraries with the Open Enclave SDK

Build and install the same version of MUSL used in Open Enclave
`
wget http://www.musl-libc.org/releases/ musl-1.1.21.tar.gz
tar zxvf musl-1.1.21.tar.gz
cd musl-1.1.21
./configure CFLAGS=-fPIC --prefix=/opt/musl --disable-shared
make
make install
`

