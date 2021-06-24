
CFLAGS := -Wall -Werror
INCLUDE := -include src/types.h

all:
.PHONY: all

test:                       \
                            \
build/types.o               \
                            \
build/byte_span.o           \
build/vector.o              \
build/circular_buffer.o     \
build/bst.o                 \
                            \
build/history.o             \
                            \
build/test/main.o           \
build/test/byte_span_test.o \
build/test/bst_test.o
	$(CC) -o $@ $^ $(LIBS)

build/types.o: src/types.c

build/vector.o: src/vector.c src/vector.h src/byte_span.h
build/byte_span.o: src/byte_span.c src/byte_span.h
build/circular_buffer.o: src/circular_buffer.c src/circular_buffer.h src/byte_span.h
build/bst.o: src/bst.c src/bst.h
build/history.o: src/history.c src/history.h src/circular_buffer.h src/byte_span.h

build/test/byte_span_test.o: src/test/byte_span_test.c src/test/byte_span_test.h src/byte_span.h src/test/lt.h
build/test/bst_test.o: src/test/bst_test.c src/test/bst_test.h src/bst.h src/test/lt.h
build/test/main.o: src/test/main.c src/test/bst_test.h

build/%.o: src/%.c src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)
