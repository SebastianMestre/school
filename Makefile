
CXXFLAGS := -Wall

INCLUDE := -include src/types.h

all:
.PHONY: all

test: \
build/test/main.o \
\
build/byte_span.o\
\
build/test/bst_test.o \
build/bst.o
	$(CC) -o $@ $^ $(LIBS)

build/byte_span.o: src/byte_span.c src/byte_span.h
build/bst.o: src/bst.c src/bst.h

build/test/bst_test.o: src/test/bst_test.c src/test/bst_test.h src/bst.h
build/test/main.o: src/test/main.c src/test/bst_test.h

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CXXFLAGS) $(INCLUDE)
