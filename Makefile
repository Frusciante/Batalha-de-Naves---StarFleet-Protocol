CC := gcc

SRC_SERVER := $(shell find src_server -name '*.c') 
SRC_CLIENT := $(shell find src_client -name '*.c') 
SRC_COMMON := $(shell find . -maxdepth 1 -name '*.c')

OBJ_SERVER := $(SRC_SERVER:.c=.o)
OBJ_CLIENT := $(SRC_CLIENT:.c=.o)
OBJ_COMMON := $(SRC_COMMON:.c=.o)

EXE_CLIENT := bin/client
EXE_SERVER := bin/server

all: $(EXE_CLIENT) $(EXE_SERVER)

%.o: %.c
	$(CC) -c $< -o $@

$(EXE_SERVER): $(OBJ_SERVER) $(OBJ_COMMON)
	@mkdir -p bin
	$(CC) $(OBJ_SERVER) $(OBJ_COMMON) -o $(EXE_SERVER)

$(EXE_CLIENT): $(OBJ_CLIENT) $(OBJ_COMMON)
	@mkdir -p bin
	$(CC) $(OBJ_CLIENT) $(OBJ_COMMON) -o $(EXE_CLIENT)

clean:
	rm -f $(EXE_CLIENT) $(EXE_SERVER) $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_COMMON)
