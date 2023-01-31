PMan: PMan.o
	gcc -o PMan PMan.o -lreadline

clean: 
	rm -f PMan PMan.o

run: PMan
	./PMan
