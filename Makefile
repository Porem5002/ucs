CC=gcc
OUT_NAME=ucs

SRCDIR=src
SRC=$(wildcard $(SRCDIR)/*.c)

CC_COMMON_FLAGS=-Wall -Wextra -Wconversion
CC_REL_FLAGS=-O2
CC_DBG_FLAGS=-g -DDTS_DEBUG_CHECKS

ifeq ($(OS),Windows_NT) # Windows
	RES_RC=resources.rc
	RES_OBJ=resources.o

	CC_COMMON_FLAGS+=-Wno-pedantic-ms-format
	CC_REL_FLAGS+=-mwindows
	SDL_FLAGS=-lmingw32 -Llib -lSDL2main -lSDL2 SDL2_image.dll SDL2_ttf.dll
else # Linux
	SDL_FLAGS=`sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf
endif

CC_REL_FLAGS+=$(CC_COMMON_FLAGS)
CC_DBG_FLAGS+=$(CC_COMMON_FLAGS)

default: debug

release: $(SRC) $(RES_OBJ)
	$(CC) $(CC_REL_FLAGS) $^ -o $(OUT_NAME) $(SDL_FLAGS)

debug: $(SRC) $(RES_OBJ)
	$(CC) $(CC_DBG_FLAGS) $^ -o $(OUT_NAME) $(SDL_FLAGS)

ifeq ($(OS),Windows_NT)
$(RES_OBJ): $(RES_RC)
	windres $^ -o $@
clean:
	del $(OUT_NAME).exe
else
clean:
	rm $(OUT_NAME)
endif