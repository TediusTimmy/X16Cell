#!/bin/bash -xe

if [[ "$1" == "" ]]; then
   #TEST=YardTest1
   TEST=YardTest2
   #TEST=StoreTest
   #TEST=YardFunTest
else
   TEST=$1
fi

./comp/bin/cc65 --target cx16 --static-locals -O -o obj/floats.s floats.c
./comp/bin/cc65 --target cx16 -O -o obj/store.s store.c
./comp/bin/cc65 --target cx16 -O -o obj/shunting.s shunting.c
./comp/bin/cc65 --target cx16 -O -o obj/machina.s machina.c
./comp/bin/cc65 -I. --target cx16 --static-locals -O -o obj/$TEST.s tests/$TEST.c

./comp/bin/ca65 -t cx16 -o obj/floats.o obj/floats.s
./comp/bin/ca65 -t cx16 -o obj/store.o obj/store.s
./comp/bin/ca65 -t cx16 -o obj/shunting.o obj/shunting.s
./comp/bin/ca65 -t cx16 -o obj/machina.o obj/machina.s
./comp/bin/ca65 -t cx16 -o obj/$TEST.o obj/$TEST.s

./comp/bin/ld65 -t cx16 -o bin/$TEST.prg obj/floats.o obj/shunting.o obj/store.o obj/machina.o obj/$TEST.o cx16.lib

./comp/emu/x16emu.exe -prg bin/$TEST.prg
