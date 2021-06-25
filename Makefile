
CFLAGS := -Wall -Werror
INCLUDE := -include src/types.h

all:
.PHONY: all

test:                       \
                            \
build/types.o               \
                            \
build/span.o                \
build/vector.o              \
build/circular_buffer.o     \
build/bst.o                 \
                            \
build/contact.o             \
                            \
build/history.o             \
build/contacts.o            \
                            \
build/test/main.o           \
build/test/span_test.o      \
build/test/bst_test.o
	$(CC) -o $@ $^ $(LIBS)

build/types.o: src/types.c

build/span.o: src/span.c src/span.h
build/vector.o: src/vector.c src/vector.h src/span.h
build/circular_buffer.o: src/circular_buffer.c src/circular_buffer.h src/span.h
build/bst.o: src/bst.c src/bst.h src/span.h

build/contact.o: src/contact.c src/contact.h

build/history.o: src/history.c src/history.h src/circular_buffer.h src/span.h
build/contacts.o: src/contacts.c src/contacts.h src/contact.h src/vector.h src/span.h

build/test/span_test.o: src/test/span_test.c src/test/span_test.h src/span.h src/test/lt.h
build/test/bst_test.o: src/test/bst_test.c src/test/bst_test.h src/bst.h src/test/lt.h
build/test/main.o: src/test/main.c src/test/bst_test.h

build/%.o: src/%.c src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)
