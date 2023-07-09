#! /bin/bash

cd ../build
cmake --build .

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd gui/muhle_tester
./muhle_tester
