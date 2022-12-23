all: umalloc.o
	@gcc -o memgrind memgrind.c umalloc.o

umalloc.o: umalloc.c
	@gcc -o umalloc.o -c umalloc.c

clean:
	@echo "Removed files"
	@rm *.o ./memgrind


