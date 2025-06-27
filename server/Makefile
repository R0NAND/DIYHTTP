CFLAGS = -Iinclude

all: server parser

server: src/server.c
	gcc -g -o server src/server.c
parser: src/parser.c src/parsing.c include/parsing.h src/util.c include/util.h include/http_enums.h
	gcc -g $(CFLAGS) -o parser src/parser.c src/parsing.c src/util.c src/headers.c
