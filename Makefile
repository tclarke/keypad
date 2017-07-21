#DEBUG=-g -DDEBUG
DEBUG=

keypad: keypad.o leds.o codes.o
	gcc -o keypad keypad.o leds.o codes.o `pkg-config --libs gslc` -lbcm2835

keypad.o: keypad.c Makefile
	gcc -std=c99 -c keypad.c `pkg-config --cflags gslc` $(DEBUG)

leds.o: leds.c Makefile
	gcc -std=c99 -c leds.c $(DEBUG)

codes.o: codes.c Makefile
	gcc -std=gnu99 -c codes.c $(DEBUG)

install: keypad
	sudo install -m 755 keypad /usr/local/bin/

clean:
	rm -f keypad keypad.o leds.o codes.o
