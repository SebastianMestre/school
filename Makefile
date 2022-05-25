CFLAGS = -pthread -lstructures -Llib -fopenmp -Iinclude -lm
NOT_TARGETS = lib/ include/

FOLDERS = $(sort $(dir $(wildcard */)))
TARGETS = $(patsubst %/, run_%, $(filter-out $(NOT_TARGETS), $(FOLDERS)))
ALL = $(TARGETS)

all: $(ALL);

run_%: %/*.c
	gcc $^ -o $@ $(CFLAGS)

clean:
	rm run_*

run: $(ALL)
	@ for exec in run_*; do echo; echo "***** $$exec *****"; ./$$exec; done

%.c: ;
