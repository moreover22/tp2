CC = gcc
CFLAGS = -g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion -Werror
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

CFILES = main.c analog.c abb.c hash.c heap.c pila.c strutil.c testing.c fechautil.c cola.c
HFILES = analog.h hash.h heap.h pila.h strutil.h abb.h testing.h pila.h fechautil.h cola.h

EXEC_S = tp2
GRUPO = G14

build_grep: $(GREPFILES)
	$(CC) $(CFLAGS) -o $(EXEC_GREP) $(GREPFILES)

grep: build_grep
	./$(EXEC_GREP)

build_dc: $(DCFILES)
	$(CC) $(CFLAGS) -o $(EXEC_DC) $(DCFILES)

build: $(CFILES)
	$(CC) $(CFLAGS) -o $(EXEC_S) $(CFILES)

run: build
	./$(EXEC_S)

valgrind: build
	valgrind $(VFLAGS) ./$(EXEC_S)

gdb: build
	gdb $(GDBFLAGS) ./$(EXEC_S)

runtime: build
	time ./$(EXEC_S)

clean:
	rm -f *.o $(EXEC_S)

zip:
	zip $(GRUPO).zip $(CFILES) $(HFILES)

entregar: build
	$(MAKE) run; test $$? -eq 0
	$(MAKE) valgrind; test $$? -eq 0
	$(MAKE) clean
	zip $(GRUPO).zip $(CFILES) $(HFILES) deps.mk
	@echo "Perfecto, no falla ni pierde memoria"
	@echo "El zip esta listo para entregar"
