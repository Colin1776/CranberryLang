compile:
	gcc src/main.c -ggdb

run: compile
	./a.exe main.cbry

r: compile
	./a.out main.cbry