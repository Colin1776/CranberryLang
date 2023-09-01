SRCS := src/main.c src/util.c src/lexer.c src/parser.c src/types.c

compile:
	gcc $(SRCS) -ggdb

winrun: compile
	./a.exe main.cbry

run: compile
	./a.out main.cbry