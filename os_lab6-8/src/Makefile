all_done: server client

server: server.cpp conection.cpp tree.cpp
	g++ server.cpp conection.cpp tree.cpp -lzmq -o server -pthread -Wl,-rpath,.

client: client.cpp conection.cpp tree.cpp
	g++ client.cpp conection.cpp tree.cpp -lzmq -o client -Wl,-rpath,.