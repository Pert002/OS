#ifndef _CONECTION_H_
#define _CONECTION_H_
#include <tuple>
#include <vector>
#include <string>
#include "zmq.h"
using namespace std;

#define UNIVERSAL_MSG -1
#define SERVER_ID -2
#define PARENT_SIGNAL -3

enum struct SocketType{
	PUB,
	SUB,
};

enum struct CommandType {
	ERROR,
	RETURN,
	CREATE_CHILD,
	REMOVE_CHILD,
	EXEC_CHILD,
};

enum struct EndpointType{
	CHILD_PUB_LEFT,
	CHILD_PUB_RIGHT,
	PARENT_PUB,
};

void* create_ctx();
void destroy_ctx(void* context);
int get_socket_type(SocketType type);
void* create_socket(void* context, SocketType type);
void close_socket(void* socket);
string create_endpoint(EndpointType type, pid_t id);
void bind_socket(void* socket, string end);
void unbind_socket(void* socket, string end);
void connect_socket(void* socket, string end);
void disconnect_socket(void* socket, string end);

#define MAX_CAP 128

class Message {
protected:
	static int counter; 
public:
	CommandType command = CommandType::ERROR;
	int to_id;
	int create_id;
	int uniq_num;
	bool to_up;
	int size = 0;
	char name[MAX_CAP];
	int val = 0;
	bool rewrite = false;
	bool error = false;
	Message();
	Message(CommandType command, int to_id, int size, char* name, bool rewrite, int val, bool error, int id);
	Message(CommandType command, int to_id, int id);
	friend bool operator==(const Message& lhs, const Message& rhs);
	int& get_create_id();
	int& get_to_id();
};

void create_zmq_msg(zmq_msg_t* zmq_msg, Message& msg);
void send_zmq_msg(void* socket, Message& msg);
Message get_zmq_msg(void* socket);

class Socket {
public:
    Socket(void* context, SocketType new_socket_type, string new_endpoint);
    ~Socket();
    void send(Message message);
    Message receive();
    void subscribe(string new_endpoint);
    string get_endpoint() const;
    void*& get_socket();
private:
    void* socket;
    SocketType socket_type;
    string endpoint;
};

#endif