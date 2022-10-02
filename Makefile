mytree : mytree.o mytreefunc.o
	gcc -o mytree mytree.o mytreefunc.o -lm

mytree.o : mytree.c
	gcc -c -o mytree.o mytree.c -lm

mytreefunc.o : mytreefunc.c
	gcc -c -o mytreefunc.o mytreefunc.c -lm

clean :
	rm *.o mytree
