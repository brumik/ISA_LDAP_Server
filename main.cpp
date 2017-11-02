#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"
#include "Decode.h"

#define E_WRONG_ARGS 1

using namespace std;

void process_request(Connection con)
{
	// Creating the database from the file given.
	try {
		Database db(con.get_filename());
	} catch (const runtime_error& e) {
		cerr << "Runtime error occurred in the child process creating the database: " << e.what() << endl;
	}
	
	try {
		string message = con.get_message();
		LDAPMessage msg = Decode().decode_to_struct(message);
		cout << "MessageID is: " << msg.MessageID << endl;
		cout << "Client Version is: " << msg.BindRequest.Version << endl;
		cout << "Client Name is: " << msg.BindRequest.Name << endl;
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred:" << e.what() << endl;
	}
	
	exit(0);
}

int main(int argc, char *argv[])
{
	// Argument processing
	long port = 389;
    string file;
    int c;
    while ((c = getopt (argc, argv, "f:p:")) != -1)
        switch (c) {
            case 'f':
                file = optarg;
                break;
            case 'p':
                port = strtol(optarg, nullptr, 10);
                break;
            case '?':
                cerr << "Invalid arguments." << endl;
                return E_WRONG_ARGS;
            default:
                return E_WRONG_ARGS;
        }
	
	// Creating the connection and trying to create the child process.
	// If cannot create the child process ends with the appropriate message.
	Connection con(port, file);
	try {
		con.server_up();
		con.accept_loop(process_request);
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred before subprocess: " << e.what() << endl;
	} catch (...) {
		cerr << "Something happened." << endl;
	}
	
    return 0;
}