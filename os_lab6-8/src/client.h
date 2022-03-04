#ifndef _CLIENT_H
#define _CLIENT_H

#include <string.h>
#include <iostream>
#include <unistd.h>
#include "conection.h"
#include <map>

using namespace std;

class Client{
private:
	int id;
	void* context;
	bool terminated;
	map <string, int> info;
public:
	Socket* c_pub_left;
	Socket* c_pub_right;
	Socket* p_pub;
	Socket* p_sub;
	Socket* l_sub;
	Socket* r_sub;
	Client(int new_id, string parent_endpoint, int new_parent_id);
	~Client();
	bool& stat();
	void send_up(Message msg);
	void send_down(Message msg);
	map <string, int> &get_map();
	Message receive();
	int get_id();
	int add_child(int id);
	int parent_id;
};

#endif