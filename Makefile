
CXXFLAGS := -Wall

INCLUDE := -include src/types.h

all:
.PHONY: all

test: \
build/test/main.o \
build/test/bst_test.o \
build/bst.o
	$(CC) $^ $(LIBS)

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CXXFLAGS) $(INCLUDE)
