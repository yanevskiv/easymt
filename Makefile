OUT = lib/libeasymt.a
CC = g++ -fdiagnostics-color=always -O2
ODIR = obj
SDIR = src
INC = -Ih

_OBJS = thread.o semfor.o region.o montor.o clinda.o msgbox.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


$(ODIR)/%.o: $(SDIR)/%.cpp Makefile
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS)
	ar rvs $(OUT) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)
