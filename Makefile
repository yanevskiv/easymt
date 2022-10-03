OUT    = lib/libeasymt.a
CC     = g++
CFLAGS = -fdiagnostics-color=always -O2
ODIR   = obj
SDIR   = src
INCDIR = -Ih
LIBDIR = -Llib
LIBS   = -leasymt

_OBJS = thread.o sem.o region.o monitor.o clinda.o msgbox.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

main: main.cpp $(OUT)
	$(CC) -pthread $(INCDIR) $(LIBDIR) -o $@ $< $(LIBS) $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp Makefile
	$(CC) -c $(INCDIR) -o $@ $< $(CFLAGS) 

$(OUT): $(OBJS)
	ar rvs $(OUT) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUT)
	rm -f main
