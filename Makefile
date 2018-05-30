HEADERS = hash_test.h

default: program

program.o: hash_test.c $(HEADERS)
	gcc -c threadpool.c -o program.o

program: program.o
	gcc program.o -o program

clean:
	-rm -f program.o
	-rm -f program
