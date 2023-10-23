SDIR= src
ODIR= output
sources := $(wildcard $(SDIR)/hw*.c)
targets := $(patsubst $(SDIR)/hw%.c, $(ODIR)/hw%.output,$(sources))

CC= gcc
CFLAGS= -O3 -std=c11 -Wall

$(SDIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/%.output: $(SDIR)/%.o
	$(CC) $(CFLAGS) -o $@ $^

all: check $(targets)

check:
	mkdir -p $(ODIR)

.PHONY: check all clean

clean:
	rm -f $(SDIR)/*.o $(ODIR)/*.output