#include <string.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <csignal>
#include "conection.h"
#include "client.h"
#include <signal.h>
#include <sys/types.h>
#include <map>
using namespace std;

Client::Client(int new_id, string parent_endpoint, int new_parent_id){
	id = new_id;
	parent_id = new_parent_id;
	context = create_ctx();
	string endpoint = create_endpoint(EndpointType::CHILD_PUB_LEFT, getpid());
	c_pub_left = new Socket(context, SocketType::PUB, endpoint);
	endpoint = create_endpoint(EndpointType::CHILD_PUB_RIGHT, getpid());
	c_pub_right = new Socket(context, SocketType::PUB, endpoint);
	endpoint = create_endpoint(EndpointType::PARENT_PUB, getpid());
	p_pub = new Socket(context, SocketType::PUB, endpoint);
	p_sub = new Socket(context, SocketType::SUB, parent_endpoint);
	l_sub = nullptr;
	r_sub = nullptr;
	terminated = false;
}

map <string, int>& Client::get_map(){
	return info;
}

Client::~Client(){
	if(terminated) 
		return;
	terminated = true;
	try{
		delete c_pub_left;
		delete c_pub_right;
		delete p_pub;
		delete p_sub;
		if(l_sub)
			delete l_sub;
		if(r_sub)
			delete r_sub;
		destroy_ctx(context);
	} 
	catch(runtime_error& err){
		cout << "Server wasn't stopped " << err.what() << endl;
	}
}

bool& Client::stat(){
	return terminated;
}

void Client::send_up(Message msg){
	msg.to_up = true;
	p_pub->send(msg);
}

void Client::send_down(Message msg){
	msg.to_up = false;
	c_pub_left->send(msg);
	c_pub_right->send(msg);
}

int Client::get_id(){
	return id;
}

int Client::add_child(int new_id){
	pid_t pid = fork();
	if(pid == -1) 
		throw runtime_error("Fork error.");
	if(!pid){
		string endpoint;
		if(new_id < id)
			endpoint = c_pub_left->get_endpoint();
		else
			endpoint = c_pub_right->get_endpoint();
		execl("client", "client", to_string(new_id).data(), endpoint.data(), to_string(id).data(), NULL);
		throw runtime_error("Execl error.");
	}
	string endpoint = create_endpoint(EndpointType::PARENT_PUB, pid);
	size_t time = 10000;
	if(id > new_id){
		l_sub = new Socket(context, SocketType::SUB, endpoint);
		zmq_setsockopt(l_sub->get_socket(), ZMQ_RCVTIMEO, &time, sizeof(time));
	} 
	else{
		r_sub = new Socket(context, SocketType::SUB, endpoint);
		zmq_setsockopt(r_sub->get_socket(), ZMQ_RCVTIMEO, &time, sizeof(time));
	}
	return pid;
}

void process_msg(Client& client, Message msg){
	if (msg.command == CommandType::ERROR)
		throw runtime_error("Error message received.");
	else if (msg.command == CommandType::RETURN){
		msg.get_to_id() = SERVER_ID;
		client.send_up(msg);
	}
	else if (msg.command == CommandType::CREATE_CHILD){
		msg.get_create_id() = client.add_child(msg.get_create_id());
		msg.get_to_id() = SERVER_ID;
		client.send_up(msg);
	}
	else if (msg.command == CommandType::REMOVE_CHILD){
		if(msg.to_up){
			client.send_up(msg);
		}
		else if(msg.to_id != client.get_id() && msg.to_id != UNIVERSAL_MSG){
			client.send_down(msg);
		}
		else{
			msg.get_to_id() = PARENT_SIGNAL;				
			msg.get_create_id() = client.get_id(); 
			client.send_up(msg);
			msg.get_to_id() = UNIVERSAL_MSG;
			client.send_down(msg);
			client.~Client();
			throw invalid_argument("Exiting child...");
		}
	}
	else if (msg.command == CommandType::EXEC_CHILD){
		string t = msg.name;
		bool error = false;
		if(msg.rewrite == false){
			if((client.get_map()).find(t) != (client.get_map()).end())
				msg.val = client.get_map()[t];
			else 
				error = true;
		} 
		else 
			client.get_map()[t] = msg.val;
		msg.error = error;
		msg.get_to_id() = SERVER_ID;
		msg.get_create_id() = client.get_id();
		client.send_up(msg);
	}
	else
		throw runtime_error("Undefined command.");
}

Client* client_ptr = nullptr;
void TerminateByUser(int) {
	if (client_ptr != nullptr)
		client_ptr->~Client();
	cout << to_string(getpid()) + "User termination" << endl;
	exit(0);
}

int main (int argc, char const *argv[]) 
{
	if(argc != 4){
		cout << "-1" << endl;
		return -1;
	}
	try{
		Client client(stoi(argv[1]), string(argv[2]), stoi(argv[3]));
		client_ptr = &client;
		cout << getpid() << ": " "Client started. "  << "Id:" << client.get_id() << endl;
		for(;;){
			Message msg = client.p_sub->receive();
			if(msg.to_id != client.get_id() && msg.to_id != UNIVERSAL_MSG){
				if(msg.to_up)
					client.send_up(msg);
				else{
					try{
						if(client.get_id() < msg.to_id){
							msg.to_up = false;
							client.c_pub_right->send(msg);
							msg = client.r_sub->receive();
						} 
						else{
							msg.to_up = false;
							client.c_pub_left->send(msg);
							msg = client.l_sub->receive();
						}
						if(msg.command == CommandType::REMOVE_CHILD && msg.to_id == PARENT_SIGNAL){
							msg.to_id = SERVER_ID;
							if(client.get_id() < msg.get_create_id()){
								delete client.r_sub;
								client.r_sub = nullptr;
							} 
							else{
								delete client.l_sub;
								client.l_sub = nullptr;
							}
						}
						client.send_up(msg);
					}
					catch(...){
						client.send_up(Message());
					}
				}
			} 
			else
				process_msg(client, msg);
		}
	} 
	catch(runtime_error& err){
		cout << getpid() << ": " << err.what() << '\n';
	} 
	catch(invalid_argument& inv){
		cout << getpid() << ": " << inv.what() << '\n';
	}
	return 0;
}
