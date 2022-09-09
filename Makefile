CLIENTS = build/text_client.o build/biny_client.o
KV_STORE = build/kv_hashtable.o build/hashtable.o build/list.o
UTILS = build/commands.o build/connections.o build/try_alloc.o build/fd_utils.o

start: build/bind.o build/connections.o build/server 
	gcc -o $@ build/bind.o build/connections.o
	
build/server: build/main.o $(CLIENTS) $(KV_STORE) $(UTILS)
	gcc -pthread -o $@ $^

clean:
	rm -rf build/
	rm -f server.out
.PHONY: clean

build/bind.o: server/bind.c server/connections.h
build/main.o: server/main.c server/connections.h server/kv_store.h server/commands.h server/text_client.h server/biny_client.h server/try_alloc.h server/fd_utils.h
build/text_client.o: server/text_client.c server/text_client.h server/commands.h server/fd_utils.h server/try_alloc.h
build/biny_client.o: server/biny_client.c server/biny_client.h server/commands.h server/fd_utils.h server/try_alloc.h
build/kv_hashtable.o: server/kv_hashtable.c server/hashtable.h server/kv_store_interface.h
build/hashtable.o: server/hashtable.c server/hashtable.h server/kv_store.h server/list.h
build/list.o: server/list.c server/list.h
build/commands.o: server/commands.c server/commands.h server/try_alloc.h server/kv_store.h
build/connections.o: server/connections.c server/connections.h
build/try_alloc.o: server/try_alloc.c server/try_alloc.h server/kv_store.h
build/fd_utils.o: server/fd_utils.c server/fd_utils.h


build/%.o: server/%.c
	@mkdir -p build
	gcc -c -o $@ $<
