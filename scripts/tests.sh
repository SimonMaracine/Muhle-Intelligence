#! /bin/bash

clear
cd ../build
cmake --build . -j 8 --target test_library

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd tests/test_library/
./test_library
