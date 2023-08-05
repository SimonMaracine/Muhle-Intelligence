#! /bin/bash

clear
cd ../build
cmake --build . -j 8

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd gui/muhle_tester
./muhle_tester
