all:
	gcc -c -g terminal.c -o terminal.o
	gcc -c -g bonus.c -o bonus.o

	gcc -o .hello *.o

clean:
	rm -r *.o
