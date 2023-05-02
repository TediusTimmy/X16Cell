CC := gcc
LFLAGS += -lcurses
#CC := x86_64-w64-mingw32-gcc
#LFLAGS += external/conio.o
LCOV := ../External/lcov/bin

CFLAGS += -Wall -Wextra -Wpedantic

T_INCLUDE := -I.

ifeq "$(MAKECMDGOALS)" "release"
   CFLAGS += -O2
endif

ifeq "$(MAKECMDGOALS)" "debug"
   CFLAGS += -O0 -g
endif

ifeq "$(MAKECMDGOALS)" "test"
   CFLAGS += -O0 -g
endif

ifeq "$(MAKECMDGOALS)" "coverage"
   CFLAGS += -O0 -g --coverage
endif

.PHONY: all clean release debug test coverage
all:
	echo There is no all there is only Zuul.

clean:
	rm bin/*.exe bin/*.gcda bin/*.gcno bin/*.info | true
	rm obj/*.o obj/*.gcda obj/*.gcno | true
	rm -rf bin/cov

release: bin/calc.exe


debug: bin/calc.exe


test: bin/AddTest.exe bin/MiscTest.exe bin/MulTest.exe bin/YardTest1.exe bin/YardTest2.exe bin/YardTest3.exe bin/StoreTest.exe
	bin/AddTest.exe
	bin/MiscTest.exe
	bin/MulTest.exe
	bin/YardTest1.exe
	bin/YardTest2.exe
	bin/YardTest3.exe
	bin/StoreTest.exe

coverage: bin/AddTest.exe bin/MiscTest.exe bin/MulTest.exe bin/YardTest1.exe bin/YardTest2.exe bin/YardTest3.exe bin/StoreTest.exe
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --capture --initial --directory obj --output-file bin/Base.info
	echo Start Test
	bin/AddTest.exe
	bin/MiscTest.exe
	bin/MulTest.exe
	bin/YardTest1.exe
	bin/YardTest2.exe
	bin/YardTest3.exe
	bin/StoreTest.exe
	echo Test Done
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --capture --directory obj --output-file bin/Run.info
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --add-tracefile bin/Base.info --add-tracefile bin/Run.info --output-file bin/Full.info
	mkdir bin/cov
	$(LCOV)/genhtml --rc lcov_branch_coverage=1 --branch-coverage --output-directory ./bin/cov bin/Full.info

bin/AddTest.exe: obj/floats.o tests/AddTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/AddTest.exe tests/AddTest.c obj/*.o

bin/MiscTest.exe: obj/floats.o tests/MiscTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/MiscTest.exe tests/MiscTest.c obj/*.o

bin/MulTest.exe: obj/floats.o tests/MulTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/MulTest.exe tests/MulTest.c obj/*.o

bin/YardTest1.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o tests/YardTest1.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/YardTest1.exe tests/YardTest1.c obj/*.o $(LFLAGS)

bin/YardTest2.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o tests/YardTest2.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/YardTest2.exe tests/YardTest2.c obj/*.o $(LFLAGS)

bin/YardTest3.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o tests/YardTest3.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/YardTest3.exe tests/YardTest3.c obj/*.o $(LFLAGS)

bin/YardFunTest.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o tests/YardFunTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/YardFunTest.exe tests/YardFunTest.c obj/*.o $(LFLAGS)

bin/StoreTest.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o tests/StoreTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/StoreTest.exe tests/StoreTest.c obj/*.o $(LFLAGS)

bin/calc.exe: obj/floats.o obj/shunting.o obj/store.o obj/machina.o obj/ui.o obj/main.o
	$(CC) $(CFLAGS) $(T_INCLUDE) -o bin/calc.exe obj/*.o $(LFLAGS)

obj/floats.o: floats.c
	$(CC) $(CFLAGS) -c -o obj/floats.o floats.c

obj/shunting.o: shunting.c
	$(CC) $(CFLAGS) -c -o obj/shunting.o shunting.c

obj/store.o: store.c
	$(CC) $(CFLAGS) -c -o obj/store.o store.c

obj/machina.o: machina.c
	$(CC) $(CFLAGS) -c -o obj/machina.o machina.c

obj/ui.o: ui.c
	$(CC) $(CFLAGS) -c -o obj/ui.o ui.c

obj/main.o: main.c
	$(CC) $(CFLAGS) -c -o obj/main.o main.c
