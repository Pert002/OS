#include "conection.h"
#include <tuple>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
using namespace std;

void* create_ctx(){
	void* context = zmq_ctx_new();
	if(context == nullptr)
		throw runtime_error("New context can't be created.");
	return context;
}

void destroy_ctx(void* context){
	sleep(1);
	if(zmq_ctx_destroy(context) != 0)
		throw runtime_error("Context can't be destroyed.");
}

int get_socket_type(SocketType type){
	if(type == SocketType::PUB)
		return ZMQ_PUB;
	if(type == SocketType::SUB)
		return ZMQ_SUB;
	throw runtime_error("Undefined socket type.");
}

void* create_socket(void* context, SocketType type){
	int zmq_type = get_socket_type(type);
	void* socket = zmq_socket(context, zmq_type);
	if(socket == nullptr)
		throw runtime_error("Can not create socket.");
	return socket;
}

void close_socket(void* socket){
	sleep(1);
	if(zmq_close(socket) != 0)
		throw runtime_error("Can not close socket.");
}

string create_endpoint(EndpointType type, pid_t id){
	if (type == EndpointType::PARENT_PUB)
		return "ipc:///tmp/p_pub" + to_string(id);
	if (type ==  EndpointType::CHILD_PUB_LEFT)
		return "ipc:///tmp/c_pub_left" + to_string(id);
	if (type ==  EndpointType::CHILD_PUB_RIGHT)
		return "ipc:///tmp/c_pub_right" + to_string(id);
	throw runtime_error("Wrong Endpoint type.");
}

void bind_socket(void* socket, string end){
	if(zmq_bind(socket, end.data()) != 0)
		throw runtime_error("Can not bind socket.");	
}

void unbind_socket(void* socket, string end){
	sleep(1);
	if(zmq_unbind(socket, end.data()) != 0)
		throw runtime_error("Can not unbind socket.");	
}

void connect_socket(void* socket, string end){
	if(zmq_connect(socket, end.data()) != 0)
		throw runtime_error("Can not connect socket.");	
	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, 0, 0);
}

void disconnect_socket(void* socket, string end){
	if(zmq_disconnect(socket, end.data()) != 0)
		throw runtime_error("Can not disconnect socket.");	
}

int Message::counter;

Message::Message(){
	command = CommandType::ERROR;
	uniq_num = counter++;
	to_up = false;
}

Message::Message(CommandType new_command, int new_to_id, int new_size, char* new_name, bool new_rewrite, int new_val, bool new_error, int new_id)
: command(new_command), 
to_id(new_to_id), 
size(new_size), 
rewrite(new_rewrite), 
val(new_val), 
error(new_error), 
uniq_num(counter++), 
to_up(false), 
create_id(new_id){
	for(int i = 0; i < size; ++i){
		name[i] = new_name[i];
	}
}

Message::Message(CommandType new_command, int new_to_id, int new_id)
: command(new_command), to_id(new_to_id), uniq_num(counter++), to_up(false), create_id(new_id){}

bool operator==(const Message& lhs, const Message& rhs){
	return tie(lhs.command, lhs.to_id, lhs.create_id, lhs.uniq_num) == tie(rhs.command, rhs.to_id, rhs.create_id, rhs.uniq_num);
}

int& Message::get_create_id(){
	return create_id;
}

int& Message::get_to_id(){
	return to_id;
}

void create_msg(zmq_msg_t* zmq_msg, Message& msg){
	zmq_msg_init_size(zmq_msg, sizeof(msg));
	memcpy(zmq_msg_data(zmq_msg), &msg, sizeof(msg));
}

void send_msg(void* socket, Message& msg){
	zmq_msg_t zmq_msg;
	create_msg(&zmq_msg, msg);
	if(!zmq_msg_send(&zmq_msg, socket, 0))
		throw runtime_error("Can not send message.");
	zmq_msg_close(&zmq_msg);
}

Message get_msg(void* socket){
	zmq_msg_t zmq_msg;
	zmq_msg_init(&zmq_msg);
	if(zmq_msg_recv(&zmq_msg, socket, 0) == -1)
		return Message();
	Message msg;
	memcpy(&msg, zmq_msg_data(&zmq_msg), sizeof(msg));
	zmq_msg_close(&zmq_msg);
	return msg;
}

Socket::Socket(void* context, SocketType new_socket_type, string new_end)
: socket_type(new_socket_type), endpoint(new_end) {
    socket = create_socket(context, new_socket_type);
    if(socket_type == SocketType::PUB)
        bind_socket(socket, new_end);
    else if(socket_type == SocketType::SUB)
        connect_socket(socket, new_end);
    else 
    	throw logic_error("Undefined connection type");
}
Socket::~Socket() {
	try{
    	if(socket_type == SocketType::PUB)
        	unbind_socket(socket, endpoint);
    	else if(socket_type == SocketType::SUB)
        	disconnect_socket(socket, endpoint);
        close_socket(socket);
    } 
    catch (exception& ex) {
        cout << "Socket wasn't closed: " << ex.what() << endl;
    }
}

void Socket::send(Message message) {
    if (socket_type == SocketType::PUB)
        send_msg(socket, message);
    else 
        throw logic_error("SUB socket can't send messages");
}

Message Socket::receive() {
    if (socket_type == SocketType::SUB)
        return get_msg(socket);
    throw logic_error("PUB socket can't receive messages");
}

void Socket::subscribe(string new_endpoint) {
    if (socket_type == SocketType::SUB) {
        connect_socket(socket, new_endpoint);
        endpoint = new_endpoint;
    } 
    else
        throw logic_error("Subscribe is only for SUB sockets");
}

string Socket::get_endpoint() const{
    return endpoint;
}

void*& Socket::get_socket(){
    return socket;
}