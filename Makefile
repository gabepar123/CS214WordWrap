ww: ww.o 
	gcc -g -Wvla -Wall -fsanitize=address,undefined -o ww ww.o

ww.o: ww.c ww.h 
	gcc -c -g -Wvla -Wall -fsanitize=address,undefined ww.c

ww.h: 
	gcc -c -g -Wvla -Wall -fsanitize=address,undefined ww.c

clean:
	rm -f *.o ww


