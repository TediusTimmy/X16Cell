#!/bin/bash -xe

./comp/bin/cc65 --target cx16 --static-locals -O -o obj/floats.s floats.c
./comp/bin/cc65 --target cx16 -O -o obj/store.s    store.c
./comp/bin/cc65 --target cx16 -O -o obj/shunting.s shunting.c
./comp/bin/cc65 --target cx16 -O -o obj/machina.s  machina.c
./comp/bin/cc65 --target cx16 -O -o obj/ui.s       ui.c
./comp/bin/cc65 --target cx16 -O -o obj/main.s     main.c

./comp/bin/ca65 -t cx16 -o obj/floats.o obj/floats.s
./comp/bin/ca65 -t cx16 -o obj/store.o obj/store.s
./comp/bin/ca65 -t cx16 -o obj/shunting.o obj/shunting.s
./comp/bin/ca65 -t cx16 -o obj/machina.o obj/machina.s
./comp/bin/ca65 -t cx16 -o obj/ui.o obj/ui.s
./comp/bin/ca65 -t cx16 -o obj/main.o obj/main.s

./comp/bin/ld65 -t cx16 -o bin/calc.prg obj/floats.o obj/shunting.o obj/store.o obj/machina.o obj/ui.o obj/main.o cx16.lib

./comp/emu/x16emu.exe -prg bin/calc.prg
