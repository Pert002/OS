#ifndef SERVER_H
#define SERVER_H

#include "tree.h"
#include "conection.h"

#define msg_wait_time 1

class Server{
public:
	Server();
	~Server();
	void print_tree();
	void send(Message msg);
	Message receive();
	void create_child(int id);
	void remove_child(int id);
	void exec_child(int id);
	pid_t get_pid();
	bool ping(int id);
	Socket*& get_pub();
	Socket*& get_sub();
	void* get_context();
	binary_tree& get_tree();
	Message last_msg;
private:
	pid_t pid;
	binary_tree t;
	void *context = nullptr;
	Socket* pub;
	Socket* sub;
	bool working;
	pthread_t receive_msg;
};

#endif