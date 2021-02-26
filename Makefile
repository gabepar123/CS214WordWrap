driver: driver.o ww.o
	gcc -g -std=c99 -Wvla -Wall -fsanitize=address,undefined -o driver driver.o ww.o

driver.o: driver.c ww.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined driver.c

ww.o: ww.c ww.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined ww.c

