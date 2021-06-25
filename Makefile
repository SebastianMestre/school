
CFLAGS := -Wall -Werror
INCLUDE := -include src/types.h

all: app test
.PHONY: all

common_objects :=           \
build/types.o               \
                            \
build/span.o                \
build/vector.o              \
build/circular_buffer.o     \
build/bst.o                 \
                            \
build/contact.o             \
                            \
build/storage.o             \
build/history.o             \
build/index.o               \
build/database.o            \

app: $(common_objects)      \
build/main.o
	$(CC) -o $@ $^ $(LIBS)

test: $(common_objects)     \
build/test/main.o           \
build/test/span_test.o      \
build/test/vector_test.o    \
build/test/bst_test.o
	$(CC) -o $@ $^ $(LIBS)

build/types.o: src/types.c

build/span.o: src/span.c src/span.h
build/vector.o: src/vector.c src/vector.h src/span.h
build/circular_buffer.o: src/circular_buffer.c src/circular_buffer.h src/span.h
build/bst.o: src/bst.c src/bst.h src/span.h

build/contact.o: src/contact.c src/contact.h

build/storage.o: src/storage.c src/storage.h src/contact.h src/vector.h src/span.h
build/history.o: src/history.c src/history.h src/storage.h src/circular_buffer.h src/vector.h src/span.h
build/index.o: src/index.c src/index.h src/storage.h src/bst.h src/vector.h src/span.h

build/database.o: src/database.c src/database.h src/index.h src/history.h src/storage.h src/circular_buffer.h src/bst.h src/vector.h src/span.h

build/test/span_test.o: src/test/span_test.c src/test/span_test.h src/span.h
build/test/vector_test.o: src/test/vector_test.c src/test/vector_test.h src/vector.h src/span.h
build/test/bst_test.o: src/test/bst_test.c src/test/bst_test.h src/bst.h
build/test/main.o: src/test/main.c src/test/bst_test.h

build/test/%_test.o: src/test/%_test.c src/test/lt.h src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

build/%.o: src/%.c src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)
