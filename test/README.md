## Build  tests
```bash
mkdir build
cd build
cmake ..
make 
ctest -V
```

Current supported tests are [here](https://github.com/openenclave/openenclave-openssl/blob/master/test/tests.supported.default).
Broken tests are [here](https://github.com/openenclave/openenclave-openssl/blob/master/test/tests.broken)

