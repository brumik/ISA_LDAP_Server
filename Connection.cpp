#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "Connection.h"

using namespace std;

Connection::Connection(long port, const string& filename)
{
	this->port = port;
	this->filename = filename;
}

string Connection::get_filename()
{
	return filename;
}

void Connection::server_up()
{
	struct sockaddr_in server;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1)
		throw runtime_error("Could not create socket");
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	if (bind(server_sock, (struct sockaddr *) &server, sizeof(server)) < 0)
		throw runtime_error("Binding failed.");
		
	listen(server_sock, 3);
}

void Connection::accept_loop(void (*callback)(Connection))
{
	struct sockaddr_in client;
	int c = sizeof(struct sockaddr_in);
	
	while ((socket_desc = accept(server_sock, (struct sockaddr *) &client, (socklen_t *) &c))) {
		if (socket_desc < 0)
			throw runtime_error("Connection failed.");
		
		pid_t pid = fork();
		if (pid == 0)
			callback(*this);
		else if (pid < 0)
			throw runtime_error("Fork failed");
	}
}

string Connection::get_message()
{
	char buffer[BUFFER_SIZE];
	long readSize = 0;
	string msg;
	
	memset(buffer, 0, BUFFER_SIZE);
	while ((readSize = recv(socket_desc, buffer, BUFFER_SIZE, 0)) > 0) {
		msg += buffer;
		memset(buffer, 0, BUFFER_SIZE);
	}
	
	if (readSize < 0)
		throw runtime_error("Bad socket transfer when receiving file.");
	
	return msg;
}

void Connection::send_message(const string &msg)
{
	while ( send(socket_desc, msg.c_str(), msg.length(), 0) != msg.length() )
		throw runtime_error("Sending failed.");
}

Connection::~Connection()
{
	close(socket_desc);
	close(server_sock);
}
