build:
	gcc *.c -g -o tema2

test: build
	valgrind --leak-check=full ./tema2

clean:
	rm tema2 output*.out