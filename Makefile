
CFLAGS := -Wall -Werror -Wextra -g # -fsanitize=address
INCLUDE := -include src/types.h
LIBS := # -lasan

all: app test
.PHONY: all

common_objects :=           \
build/types.o               \
build/string.o              \
                            \
build/iter.o                \
build/span.o                \
build/array.o               \
build/vector.o              \
build/circular_buffer.o     \
build/bst.o                 \
build/slot_map.o            \
                            \
build/contact.o             \
                            \
build/storage.o             \
build/history.o             \
build/index.o               \
build/database.o            \
build/search_by_sum.o       \
build/quicksort.o           \

app: $(common_objects)      \
build/io.o                  \
build/main.o
	$(CC) -o $@ $^ $(LIBS)

test: $(common_objects)     \
build/test/main.o           \
build/test/span_test.o      \
build/test/vector_test.o    \
build/test/slot_map_test.o  \
build/test/bst_test.o       \
build/test/database_test.o  \
build/test/search_by_sum_test.o \
build/test/quicksort_test.o
	$(CC) -o $@ $^ $(LIBS)

build/types.o: src/types.c
build/span.o: src/span.c src/span.h
build/iter.o: src/iter.c src/iter.h
build/array.o: src/array.c src/span.h src/iter.h
build/vector.o: src/vector.c src/vector.h src/array.h src/span.h
build/circular_buffer.o: src/circular_buffer.c src/circular_buffer.h src/span.h
build/bst.o: src/bst.c src/bst.h src/span.h
build/slot_map.o: src/slot_map.c src/slot_map.h src/vector.h src/span.h

build/contact.o: src/contact.c src/contact.h

build/storage.o: src/storage.c src/storage.h src/contact.h src/slot_map.h src/vector.h src/span.h
build/history.o: src/history.c src/history.h src/storage.h src/circular_buffer.h src/vector.h      \
src/span.h
build/index.o: src/index.c src/index.h src/storage.h src/bst.h src/vector.h src/span.h

build/database.o: src/database.c src/database.h src/index.h src/history.h src/storage.h            \
src/slot_map.h src/circular_buffer.h src/bst.h src/vector.h src/span.h src/string.h
build/search_by_sum.o: src/search_by_sum.c src/search_by_sum.h src/vector.h src/span.h
build/quicksort.o: src/quicksort.c src/quicksort.h src/array.h src/span.h src/iter.h

build/io.o: src/io.c src/io.h
build/main.o: src/main.c src/io.h src/string.h src/database.h src/index.h src/history.h            \
src/storage.h src/slot_map.h src/circular_buffer.h src/bst.h src/vector.h src/span.h

build/test/span_test.o: src/test/span_test.c src/test/span_test.h src/span.h
build/test/vector_test.o: src/test/vector_test.c src/test/vector_test.h src/vector.h src/span.h
build/test/bst_test.o: src/test/bst_test.c src/test/bst_test.h src/bst.h
build/test/slot_map_test.o: src/test/slot_map_test.c src/test/slot_map_test.h src/slot_map.h
build/test/database_test.o: src/test/database_test.c src/test/database_test.h src/database.h       \
src/index.h src/history.h src/storage.h src/vector.h src/circular_buffer.h src/bst.h src/string.h  \
src/span.h
build/test/search_by_sum_test.o: src/test/search_by_sum_test.c src/test/search_by_sum_test.h       \
src/vector.h src/span.h
build/test/quicksort_test.o: src/test/quicksort_test.c src/test/quicksort_test.h src/quicksort.h   \
src/span.h
build/test/main.o: src/test/main.c src/test/bst_test.h src/test/span_test.h src/test/vector_test.h \
src/test/slot_map_test.h src/test/search_by_sum_test.h

build/test/%_test.o: src/test/%_test.c src/test/lt.h src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)

build/%.o: src/%.c src/types.h
	mkdir -p $(dir $@)
	$(CC) -o $@ -c $< $(CFLAGS) $(CPPFLAGS) $(INCLUDE)
