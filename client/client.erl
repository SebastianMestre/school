-module(client).
-export([start/1, put/3, del/2, get/2, get/3, take/2, take/3, stats/1, stats/2]).

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

start(Address) ->
	% TODO cambiar el puerto a 889
	case gen_tcp:connect(Address, 8001, [binary, {active, false}, {packet,raw}]) of
		{ok, Socket} ->  	
			HandlePID = spawn(fun() -> handle_conn(Socket) end),
			{ok, {connID, HandlePID}};
		{error, Reason} -> {noConn, Reason}
	end.

handle_conn(Socket) ->
	receive
		{put, K, V, PID} ->
			gen_tcp:send(Socket, binary_query(?PUT, K, V)),
			case receive_code(Socket) of
				{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
				Error -> Error
			end;
		{del, K, PID} -> 
			gen_tcp:send(Socket, binary_query(?DEL, K)),
			case receive_code(Socket) of
				{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
				Error -> Error
			end;
		{get, K, PID} ->
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
			end;
		{take, K, PID} ->
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
			end;
		{stats, PID} -> 
			gen_tcp:send(Socket, <<?STATS>>),
			case receive_code(Socket) of
				{ok, ok} -> case receive_val(Socket) of
					{ok, Val} -> 
						PID ! {answer, ok, Val}, 
						handle_conn(Socket);
					Error -> Error
				end;
				{ok, Code} -> PID ! {answer, Code}, handle_conn(Socket);
				Error -> Error
			end;
		close -> 
			io:fwrite("close ~p~n", [Socket]),
			gen_tcp:shutdown(Socket, read_write),
			ok
	end.

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

decode(?OK) -> ok;
decode(?EINVAL) -> einval;
decode(?ENOTFOUND) -> enotfound;
decode(?EBINARY) -> ebinary;
decode(?EBIG) -> ebig;
decode(?EUNK) -> eunk;
decode(_) 	-> unknown.

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

put({connID, HandlePID}, K, V) ->
	HandlePID ! {put, K, V, self()},
	receive
		{answer, ok} -> ok;
		{answer, code} -> {error, code};
		Error -> Error
	end.

del({connID, HandlePID}, K) ->
	HandlePID ! {del, K, self()},
	receive
		{answer, ok} -> ok;
		{answer, enotfound} -> enotfound;
		{answer, code} -> {error, code};
		Error -> Error
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

% Devuelve la respuesta en binario.
% MAYBE parsearla? no entiendo bien cual seria el formato 	
stats(Conn) -> stats(Conn, infinity).
stats({connID, HandlePID}, Timeout) ->
	HandlePID ! {stats, self()},
	receive
		{answer, ok, Val} -> {ok, Val};
		{answer, code} -> {error, code};
		Error -> Error
	after Timeout -> timeout
	end.