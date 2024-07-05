#! /bin/bash

./build.sh muhle_tester

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ../build/muhle_tester
./muhle_tester
