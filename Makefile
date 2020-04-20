all:
	gcc -lncurses mines.c -o mines

install:
	cp mines /usr/local/bin

clean:
	rm -f mines

uninstall:
	rm -f /usr/local/bin/mines
