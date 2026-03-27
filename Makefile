TARGET=my_db.exe
SDIR=./src
IDIR=./include
ODIR=./obj
CC=gcc

LIBS=
CFLAGS=-Wall $(patsubst %,-I%,$(IDIR)) -D_GNU_SOURCE -D_DEBUG -lc

OBJ = $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(wildcard $(SDIR)/*.c))
INC = $(wildcard $(IDIR)/*.h)

.PHONY: default clean
default: $(TARGET)

$(ODIR):
	mkdir $@

$(ODIR)/%.o: $(SDIR)/%.c $(INC) | $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ) $(INC) | $(ODIR)
	$(CC) $(OBJ) $(CFlAGS) $(LIBS) -o $@

clean:
	rm -f $(ODIR)/*.o $(TARGET)
