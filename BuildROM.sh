#!/bin/bash -xe

if [[ "$1" == "" ]]; then
   #TEST=AddTest
   #TEST=MiscTest
   TEST=MulTest
else
   TEST=$1
fi

./comp/bin/cc65 --target cx16 --static-locals -O -o obj/floats.s floats.c
./comp/bin/cc65 -I. --target cx16 -O -o obj/$TEST.s tests/$TEST.c
./comp/bin/ca65 -t cx16 -o obj/floats.o obj/floats.s
./comp/bin/ca65 -t cx16 -o obj/$TEST.o obj/$TEST.s
./comp/bin/ld65 -t cx16 -o bin/$TEST.prg obj/floats.o obj/$TEST.o cx16.lib

./comp/emu/x16emu.exe -prg bin/$TEST.prg
