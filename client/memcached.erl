-module(memcached).
-export([start/1,
		put/3,
		put/4,
		del/2, 
		del/3,
		get/2, 
		get/3, 
		take/2,
		take/3, 
		stats/1,
		stats/2,
		close/1,
		close/2]).

-define(PUT, 11).
-define(DEL, 12).
-define(GET, 13).
-define(TAKE, 14).
-define(STATS, 21).
-define(OK, 101).
-define(EINVAL, 111).
-define(ENOTFOUND, 112).
-define(EBINARY, 113).
-define(EBIG, 114).
-define(EUNK, 115).
-define(EOOM, 116).

% inicia conexion
start(Address) ->
	% TODO cambiar el puerto a 889
	case gen_tcp:connect(Address, 8001, [binary, {active, false}, {packet,raw}]) of
		{ok, Socket} ->  	
			HandlePID = spawn(fun() -> handle_conn(Socket) end),
			{ok, {connID, HandlePID}};
		{error, Reason} -> {noConn, Reason}
	end.

% maneja conexion
handle_conn(Socket) ->
	receive
		{put, K, V, PID} 	-> handle_put(Socket, K, V, PID);
		{del, K, PID} 		-> handle_del(Socket, K, PID);
		{get, K, PID} 		-> handle_get(Socket, K, PID);
		{take, K, PID} 		-> handle_take(Socket, K, PID);
		{stats, PID} 		-> handle_stats(Socket, PID);
		{close, PID} 		-> handle_close(Socket, PID)
	end.

% maneja comandos
handle_put(Socket, K, V, PID) ->
	gen_tcp:send(Socket, binary_query(?PUT, K, V)),
	case receive_code(Socket) of
		{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
		Error -> Error
	end.
handle_del(Socket, K, PID) ->
	gen_tcp:send(Socket, binary_query(?DEL, K)),
	case receive_code(Socket) of
		{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
		Error -> Error
	end.
handle_get(Socket, K, PID) ->
	gen_tcp:send(Socket, binary_query(?GET, K)),
	case receive_code(Socket) of
		{ok, ok} -> case receive_val(Socket) of
			{ok, Val} -> 
				PID ! {answer, ok, binary_to_term(Val)}, 
				handle_conn(Socket);
			Error -> Error
		end;
		{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
		Error -> Error
	end.
handle_take(Socket, K, PID) ->
	gen_tcp:send(Socket, binary_query(?TAKE, K)),
	case receive_code(Socket) of
		{ok, ok} -> case receive_val(Socket) of
			{ok, Val} -> 
				PID ! {answer, ok, binary_to_term(Val)}, 
				handle_conn(Socket);
			Error -> Error
		end;
		{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
		Error -> Error
	end.
handle_stats(Socket, PID) ->
	gen_tcp:send(Socket, <<?STATS>>),
	case receive_code(Socket) of
		{ok, ok} -> case receive_val(Socket) of
			{ok, Val} -> 
				PID ! {answer, ok, binary_to_list(Val)}, 
				handle_conn(Socket);
			Error -> Error
		end;
		{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
		Error -> Error
	end.
handle_close(Socket, PID) ->
	case gen_tcp:shutdown(Socket, read_write) of
		% cerramos el socket con exito
		ok -> 
			io:fwrite("close ~p~n", [Socket]),
			PID ! closed;
		% si no pudimos cerrar el socket, seguimos en el loop
		Error -> 
			PID ! Error,
			handle_conn(Socket)	
	end.

% recibe del servidor
receive_code(Socket) ->
	case gen_tcp:recv(Socket, 1) of
		{ok, <<Code>>} -> {ok, decode(Code)};
		Error -> Error
	end.

receive_val(Socket) ->
	case gen_tcp:recv(Socket, 4) of
		{ok, BSize} -> gen_tcp:recv(Socket, binary:decode_unsigned(BSize));
		Error -> Error
	end.

% interpreta resultados
decode(?OK) -> ok;
decode(?EINVAL) -> einval;
decode(?ENOTFOUND) -> enotfound;
decode(?EBINARY) -> ebinary;
decode(?EBIG) -> ebig;
decode(?EUNK) -> eunk;
decode(?EOOM) -> eoom;
decode(_) 	-> unknown.

% construye querys
binary_query(ID, Key) ->
	BQuery = term_to_query(Key),
	<<ID, BQuery/binary>>.

binary_query(ID, Key, Val) ->
	BKey = term_to_query(Key),  
	BVal = term_to_query(Val),
	<<ID, BKey/binary, BVal/binary>>.

term_to_query(Term) ->
	BTerm = term_to_binary(Term),
	Term_size = byte_size(BTerm),
	BTerm_size = <<Term_size:32>>,
	<<BTerm_size/binary, BTerm/binary>>.

% comandos de la cache
put(Conn, K, V) -> put(Conn, K, V, infinity).
put({connID, HandlePID}, K, V, Timeout) ->
	HandlePID ! {put, K, V, self()},
	receive
		{answer, ok} -> ok;
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.

del(Conn, K) -> del(Conn, K, infinity).
del({connID, HandlePID}, K, Timeout) ->
	HandlePID ! {del, K, self()},
	receive
		{answer, ok} -> ok;
		{answer, enotfound} -> enotfound;
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.

get(Conn, K) -> get(Conn, K, infinity).
get({connID, HandlePID}, K, Timeout) ->
	HandlePID ! {get, K, self()},
	receive
		{answer, ok, Val} -> {ok, Val};
		{answer, enotfound} -> enotfound;
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.

take(Conn, K) -> take(Conn, K, infinity).
take({connID, HandlePID}, K, Timeout) ->
	HandlePID ! {take, K, self()},
	receive
		{answer, ok, Val} -> {ok, Val};
		{answer, enotfound} -> enotfound;
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.

stats(Conn) -> stats(Conn, infinity).
stats({connID, HandlePID}, Timeout) ->
	HandlePID ! {stats, self()},
	receive
		{answer, ok, Val} -> {ok, Val};
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.
% cerrar conexion
close(Conn) -> close(Conn, infinity).
close({connID, HandlePID}, Timeout) -> 
	HandlePID ! {close, self()},
	receive
		closed -> ok;
		Error -> Error
	after Timeout -> timeout
	end.
		