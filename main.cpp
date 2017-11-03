#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"
#include "Decode.h"
#include "Encode.h"

#define EXIT_SUCCESS    0
#define E_WRONG_ARGS    1
#define E_RUNTIME_ERROR 2

using namespace std;

void process_request(Connection con)
{
	// Creating the database from the file given.
	try {
		Database db(con.get_filename());
	} catch (const runtime_error& e) {
		cerr << "Runtime error occurred in the child process creating the database: " << e.what() << endl;
		exit(E_RUNTIME_ERROR);
	}
	
	// Variables used
	Decode decoder;
	Encode encoder;
	LDAPMessage ldapMessage_rec, ldapMessage_send;
	string msg;
	
	try {
		msg = con.get_message();
		ldapMessage_rec = decoder.decode_to_struct(msg);
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred:" << e.what() << endl;
	}
	
	// Creating and sending response
	if ( ldapMessage_rec.MessageType == Codes::BindRequest ) {
		ldapMessage_send.MessageID = ldapMessage_rec.MessageID;
		ldapMessage_send.MessageType = Codes::BindResponse;
		
		ldapMessage_send.BindResponse.ResultCode = decoder.get_error().get_code();
		// Todo Matched DN - what to do with it?
		ldapMessage_send.BindResponse.MatchedDN = ldapMessage_rec.BindRequest.Name;
		ldapMessage_send.BindResponse.ErrorMessage = decoder.get_error().get_message();
		cout << encoder.structure_to_hex(ldapMessage_send) << endl;
	}
	
	exit(EXIT_SUCCESS);
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