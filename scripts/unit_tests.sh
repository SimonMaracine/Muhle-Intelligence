#! /bin/bash

./build.sh test_library

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ../build/tests/test_library/
./test_library
