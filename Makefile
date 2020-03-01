

EXE_DIR = .
EXE = $(EXE_DIR)/mem

SRC= memory_management.c

# generic build details
# -std=gnu99

CC=gcc
CFLAGS=-Wextra -g -std=c99 -Wall $(OPTLEVEL)

# compile to  object code

OBJ= $(SRC:.c=.o)

.c.o:
	$(CC) $(COPT) -c -o $@ $<

# build executable

$(EXE): $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o  $(EXE)

# clean up compilation

clean:
	rm -f $(OBJ) $(EXE)

# dependencies

# main.o:  main.c  writeTree.c buildTree.c
memory_management.o: memory_management.c memory_management.h
