all:
	g++ -Isrc/Include -Lsrc/lib -o main src/main.c -lmingw32 -lSDL2main -lSDL2 