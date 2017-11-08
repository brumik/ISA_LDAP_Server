#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <vector>
#include <fcntl.h>
#include "Connection.h"

using namespace std;

Connection::Connection(long port)
{
	this->port = port;
}

void Connection::server_up()
{
	sockaddr_in6 server;
	memset(&server, 0, sizeof(server));
	
	server_sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (server_sock == -1)
		throw runtime_error("Could not create socket");
	
	server.sin6_family = AF_INET6;
	server.sin6_addr = IN6ADDR_ANY_INIT;
	server.sin6_port = htons((uint16_t)port);
	
	if (bind(server_sock, (struct sockaddr *) &server, sizeof(server)) < 0)
		throw runtime_error(string("Binding failed on port: ") + to_string(port));
		
	listen(server_sock, 3);
}

void Connection::accept_loop(void (*callback)(Connection))
{
	while ( (socket_desc = accept(server_sock, nullptr, nullptr)) ) {
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
	unsigned char buffer[BUFFER_SIZE];
	long readSize = 0;
	stringstream msg;
	
	fcntl(socket_desc, F_SETFL, O_NONBLOCK);
	
	memset(buffer, 0, BUFFER_SIZE);
	while (true) {
		
		// Receive and throwing error if something goes wrong.
		if ( (readSize = recv(socket_desc, buffer, BUFFER_SIZE, 0)) < 0 ) {
			if ( errno == EAGAIN || errno == EWOULDBLOCK )
				continue;
			else
				throw runtime_error("Bad socket transfer when receiving file.");
		}
		
		// Converting the buffer into hex string.
		for (int i = 0; i < readSize; i++)
			msg << setfill('0') << setw(2) << hex << bitset<8>(buffer[i]).to_ulong();
		
		// Reset buffer
		memset(buffer, 0, BUFFER_SIZE);
		
		// If the read was smaller than the buffer or there was no read data the message is ended.
		if ( readSize < BUFFER_SIZE || readSize ) break;
	}
	
	
	return msg.str();
}

vector<unsigned char> Connection::hex_to_bytes(const string& hex)
{
	vector<unsigned char> bytes;
	unsigned char byte;
	string byteStr;
	
	for (unsigned int i = 0; i < hex.length(); i += 2) {
		byteStr = hex.substr(i, 2);
		byte = (unsigned char)strtol(byteStr.c_str(), nullptr, 16);
		bytes.push_back(byte);
	}
	
	return bytes;
}

void Connection::send_message(const string &msg)
{
	vector<unsigned char> message = hex_to_bytes(msg);
	while ( (size_t)send(socket_desc, &message[0], message.size(), 0) != message.size() )
		throw runtime_error("Sending failed.");
	
}

void Connection::close_sockets()
{
	close(socket_desc);
	close(server_sock);
}

Connection::~Connection()
{
	this->close_sockets();
}