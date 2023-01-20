#!/bin/bash

TARGET=' -DCMAKE_EXPORT_COMPILE_COMMANDS=1 '


echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
echo "[-c :build client demo]  [-s :bulid server demo] [-w :use wbl log]"
echo "[-r :release verision]  [-d :debug verision] [-v :open complie log]"
echo "[-o :use openssl1.0.2]  [-p :use sctp datachannel]"
echo "[-u :enable udpchannel to build in demo -client_demo_udpchannel.cpp]"
echo "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"

while getopts ":cswdvroubtxp" opt
do
    case $opt in
        d)
        echo "-> build debug verision"
        TARGET+=' -DCMAKE_BUILD_TYPE=Debug'
        ;;
        r)
        echo "-> build release verision"
        TARGET+=' -DCMAKE_BUILD_TYPE=Release'
        ;;
        v)
        echo "open complie log"
        TARGET+=' -DCMAKE_VERBOSE_MAKEFILE=ON'
        ;;
        ?)
        echo "-c build client demo  -s bulid server demo -w use wbl log"
        exit 1;;
    esac
done

echo "${TARGET}" 

mkdir -p ./build

cd ./build || exit 
#rm -rf *
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 "${TARGET}" ../; make -j16;
