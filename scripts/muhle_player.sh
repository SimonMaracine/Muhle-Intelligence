#! /bin/bash

./build.sh muhle_player

if [ "$?" -ne 0 ]; then
    exit 1
fi

cd ../build/gui/muhle_player
./muhle_player
