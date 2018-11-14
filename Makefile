CC = gcc
CFLAGS = -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror 
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes

CFILES = main_abb.c abb.c pruebas_alumno.c testing.c pila.c
HFILES = abb.h testing.h pila.h

EXEC_S = abb
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
	zip $(GRUPO).zip $(CFILES) $(HFILES)
	@echo "Perfecto, no falla ni pierde memoria"
	@echo "El zip esta listo para entregar"
