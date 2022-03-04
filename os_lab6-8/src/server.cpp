#include <string.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <csignal>
#include "server.h"
#include "conection.h"

void* sub_thread(void* server){
	Server* ptr = (Server*) server;
	pid_t serv_pid = ptr->get_pid();
	try{
		pid_t child_pid = fork();
		if(child_pid == -1) 
			throw runtime_error("Can not fork.");
		if(child_pid == 0){
			execl("client", "client", "0", ptr->get_pub()->get_endpoint().data(), "-1", nullptr);
			throw runtime_error("Can not execl");
			ptr->~Server();
			return (void*)-1;
		}
		string endpoint = create_endpoint(EndpointType::PARENT_PUB, child_pid);
		ptr->get_sub() = new Socket(ptr->get_context(), SocketType::SUB, endpoint);
		ptr->get_tree().add(0);
		for(;;){
			Message msg = ptr->get_sub()->receive();
			if(msg.command == CommandType::ERROR){
				cout << "ERROR" << endl;
				exit(-1);
			}
				ptr->last_msg = msg;
			if(msg.command == CommandType::CREATE_CHILD)
				cout << "OK:" << msg.get_create_id() << endl;
			else if (msg.command == CommandType::REMOVE_CHILD){
				cout << "OK" << endl;
				ptr->get_tree().delete_el(msg.get_create_id());
			}
			else if (msg.command == CommandType::EXEC_CHILD){
				if(!msg.error)
					cout << "OK:" << msg.get_create_id() << ":" << msg.name << ":" << msg.val << endl;
				else 
					cout << "OK:" << msg.get_create_id() << ":" << "'" << msg.name << "'" << "not found" << endl;
			}
		}
	}
	catch(runtime_error& err){
		cout << "Server wasn't started " << err.what() << endl;
	}
	return nullptr;
}

Server::Server(){
	context = create_ctx();
	pid = getpid();
	string endpoint = create_endpoint(EndpointType::CHILD_PUB_LEFT, getpid());
	pub = new Socket(context, SocketType::PUB, endpoint);
	if(pthread_create(&receive_msg, 0, sub_thread, this) != 0)
		throw runtime_error("Can not run second thread.");
	working = true;
}

Server::~Server(){
	if(!working) 
		return;
	working = false;
	send(Message(CommandType::REMOVE_CHILD, 0, 0));
	try{
		delete pub;
		delete sub;
		pub = nullptr;
		sub = nullptr;
		destroy_ctx(context);
		sleep(2);
	} 
	catch (runtime_error &err){
		cout << "Server wasn't stopped " << err.what() << endl;
	}
}

pid_t Server::get_pid(){
	return pid;
}

void Server::print_tree(){
	t.print();
}

bool Server::ping(int id){
	Message msg(CommandType::RETURN, id, 0);
	send(msg);
	sleep(msg_wait_time);
	msg.get_to_id() = SERVER_ID;
	return last_msg == msg;
}

void Server::send(Message msg){
	msg.to_up = false;
	pub->send(msg);
}

Message Server::receive(){
	return sub->receive();
}

Socket*& Server::get_pub(){
	return pub;
}
	
Socket*& Server::get_sub(){
	return sub;
}

void* Server::get_context(){
	return context;
}

binary_tree& Server::get_tree(){
	return t;
}

void Server::create_child(int id){
	if(t.find(id))
		throw runtime_error("Error:" + to_string(id) + ":Node with that number already exists.");
	if(t.get_place(id) && !ping(t.get_place(id)))
		throw runtime_error("Error:" + to_string(id) + ":Parent node is unavailable.");
	send(Message(CommandType::CREATE_CHILD, t.get_place(id), id));
	t.add(id);
}

void Server::remove_child(int id){
	if(!t.find(id))
		throw runtime_error("Error:" + to_string(id) + ":Node with that number doesn't exist.");
	if(!ping(id))
		throw runtime_error("Error:" + to_string(id) + ":Node is unavailable.");
	send(Message(CommandType::REMOVE_CHILD, id, 0));
}

void Server::exec_child(int id){
	char name[MAX_CAP]; 
	char p;
	cin >> p;
	int k = 0;
	while(p!=' ' && p != '\n'){
		name[k]=p;
		p = getchar();
		k++;
	}
	name[k]='\0';
	k++;
	int val = 0;
	bool rewrite = false;
	if(p == ' '){
		cin >> val;
		rewrite = true;
	}
	if(!t.find(id))
		throw runtime_error("Error:" + to_string(id) + ":Node with that number doesn't exist.");
	if(!ping(id))
		throw runtime_error("Error:" + to_string(id) + ":Node is unavailable.");
	send(Message(CommandType::EXEC_CHILD, id, k, name, rewrite, val, false, 0));
}

void process_cmd(Server& server, string cmd){
	if(cmd == "create"){
		int id;
		cin >> id;
		if(id == 0)
			throw runtime_error("Root can't be removed.");
		server.create_child(id);
	} 
	else if(cmd == "remove"){
		int id;
		cin >> id;
		server.remove_child(id);
	} 
	else if(cmd == "exec"){
		int id;
		cin >> id;
		server.exec_child(id);
	} 
	else if(cmd == "exit")
		throw invalid_argument("Exiting...");
	else if(cmd == "ping"){
		int id;
		cin >> id;
		if(!server.get_tree().find(id))
			throw runtime_error("Error:" + to_string(id) + ":Node with that number doesn't exist.");
		if(server.ping(id))
			cout << "OK" << endl;
		else
			cout << "Node is unavailable" << endl;
	} 
	else 
		cout << "It is not a command!\n";
}

Server* ptr = nullptr;
void TerminateByUser(int) {
	if (ptr != nullptr)
		ptr->~Server();
	cout << to_string(getpid()) + " Terminated by user" << endl;
	exit(0);
}

int main (int argc, char const *argv[]) 
{
	try{
		Server server;
		ptr = &server;
		cout << getpid() << " server started correctly!\n";
		for(;;){
			try{
				string cmd;
				while(cin >> cmd)
					process_cmd(server, cmd);
			} 
			catch(const runtime_error& arg){
				cout << arg.what() << endl;
			}
		}
	} 
	catch(const runtime_error& arg){
		cout << arg.what() << endl;
	}
	catch(...){}
	sleep(5);
	return 0;
}