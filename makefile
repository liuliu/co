LDFLAGS := -lm
CFLAGS := -O3 -ffast-math -Wall -I"." $(CFLAGS)

SRCS := co.c

TARGETS = example

SRC_OBJS := $(patsubst %.c,%.o,$(SRCS))

# Make nnc/libnnc.o a phony target therefore it will be triggered every time.
.PHONY: release all lib clean dep

release: all

include scheme.mk

all: $(TARGETS)

$(TARGETS): %: %.o $(SRC_OBJS)
	$(CC) -o $@ $< co.o $(LDFLAGS)

clean:
	rm -f $(TARGETS) *.o

%.o: %.c
	$(CC) $< -o $@ -c $(CFLAGS)

dep: .dep.mk
.dep.mk: $(SRCS)
	echo '' > .dep.mk
	for SRC in $(patsubst %.cu,,$^) ; do \
		$(CC) $(CFLAGS) -MM $$SRC | sed -e 's/^.*\://g' | (echo "$${SRC%%.*}.o: \\" && cat) >> .dep.mk ; \
	done

-include .dep.mk
