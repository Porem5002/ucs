CC=gcc
CC_FLAGS=-O2 -Wall -mwindows
SDL_FLAGS=-lmingw32 -Llib -lSDL2main -lSDL2 SDL2_image.dll SDL2_ttf.dll

SRCDIR=src
SRC=$(wildcard $(SRCDIR)/*.c)
RC=resource

ucs: $(SRC) resources.o
	$(CC) $(CC_FLAGS) $^ -o $@ $(SDL_FLAGS)

debug: CC_FLAGS=-g -Wall -DDTS_DEBUG_CHECKS
debug: ucs

resources.o: resources.rc
	windres $^ -o $@

clean:
	del checkers.exe