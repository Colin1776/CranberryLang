compile:
	gcc src/main.c

run: compile
	./a.exe main.cbry

r: compile
	./a.out main.cbry