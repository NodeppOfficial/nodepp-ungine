#!/bin/bash

mkdir -p ./test/build

if [ ! -d "./test/build/CMakeFiles" ] ; then
   ( cd ./test/build; cmake .. )
fi

( cd ./test/build; make ); 

if [ ! $? -eq 0 ]; then
    echo "exit error"; exit;
fi

( cd ./test ; ./build/main )