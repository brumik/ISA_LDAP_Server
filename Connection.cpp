#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <vector>
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

	// TODO Non blocking server.
	
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1)
		throw runtime_error("Could not create socket");
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons((uint16_t)port);
	
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
	unsigned char buffer[BUFFER_SIZE];
	long readSize = 0;
	stringstream msg;
	
	memset(buffer, 0, BUFFER_SIZE);
	while ((readSize = recv(socket_desc, buffer, BUFFER_SIZE, 0)) > 0) {
		
		// Converting the buffer into hex string.
		for (int i = 0; i < readSize; i++)
			msg << setfill('0') << setw(2) << hex << bitset<8>(buffer[i]).to_ulong();
		
		// Reset buffer
		memset(buffer, 0, BUFFER_SIZE);
		
		// If the read was smaller than the buffer the message is ended.
		if ( readSize <= BUFFER_SIZE ) break;
	}
	
	if (readSize < 0)
		throw runtime_error("Bad socket transfer when receiving file.");
	
	return msg.str();
}

vector<unsigned char> Connection::hex_to_bytes(const string& hex) {
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

Connection::~Connection()
{
	close(socket_desc);
	close(server_sock);
}