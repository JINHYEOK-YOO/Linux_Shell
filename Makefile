OBJF = main.o signalsetup.o executepipe.o executeredirect.o makeargv.o myls.o
sh: $(OBJF)
	gcc -o sh $(OBJF)
main: main.c getch.h
	gcc -c main.c
signalsetup: signalsetup.c
	gcc -c signalsetup.c
executepipe: executepipe.c
	gcc -c executepipe.c
executeredirect: excuteredirect.c
	gcc -c executeredirect.c
makeargv: makeargv.c
	gcc -c makeargv.c
myls: myls.c
	gcc -c myls.c
clean:
	rm -f sh $(OBJF)
