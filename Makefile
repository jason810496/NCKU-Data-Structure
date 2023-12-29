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

# for debug
# make hw2_p1 IN=1 to run test/hw2_p1/input_01.txt 
hw2_p1:all
	./$(ODIR)/hw2_p1.output 1 < test/hw2_p1/input_0${IN}.txt

# for debug
# make hw2_p2 IN=1 to run test/hw2_p2/input_01.txt
hw2_p2:all
	./$(ODIR)/hw2_p2.output 2 < test/hw2_p2/input_0${IN}.txt


.PHONY: check all clean

clean:
	rm -f $(SDIR)/*.o $(ODIR)/*.output