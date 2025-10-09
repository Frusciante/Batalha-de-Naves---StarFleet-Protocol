CC := gcc

CFLAGS := -I. 

SRC_SERVER := $(shell find src_server -name '*.c')
SRC_CLIENT := $(shell find src_client -name '*.c')

EXE_CLIENT := bin/client
EXE_SERVER := bin/server

all: $(EXE_CLIENT) $(EXE_SERVER)

$(EXE_SERVER): $(SRC_SERVER)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC_SERVER) -o $(EXE_SERVER)

$(EXE_CLIENT): $(SRC_CLIENT)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC_CLIENT) -o $(EXE_CLIENT)

clean:
	rm -f $(EXE_CLIENT) $(EXE_SERVER)

