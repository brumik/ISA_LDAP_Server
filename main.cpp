#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"
#include "Decode.h"
#include "Encode.h"
#include "Logger.h"

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
	LDAPMessage_t ldapMessage_rec, ldapMessage_send;
	string msg;
	
	try {
		msg = con.get_message();
		ldapMessage_rec = decoder.decode_to_struct(msg);
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred: " << e.what() << endl;
		exit(E_RUNTIME_ERROR);
	}
	
	// Creating and sending response
	if ( ldapMessage_rec.MessageType == LDAPMessageType_t::BindRequest ) {
		ldapMessage_send.MessageID = ldapMessage_rec.MessageID;
		ldapMessage_send.MessageType = LDAPMessageType_t::BindResponse;
		
		ldapMessage_send.LDAPResult.ResultCode = decoder.get_error().get_code();
		ldapMessage_send.LDAPResult.MatchedDN = "";
		ldapMessage_send.LDAPResult.ErrorMessage = decoder.get_error().get_message();
		msg = encoder.struct_to_hex(ldapMessage_send);
		con.send_message(msg);
	}
	
	// Search Request
	try {
		msg = con.get_message();
		ldapMessage_rec = decoder.decode_to_struct(msg);
	} catch (runtime_error &e) {
		cerr << "Runtime Error occurred: " << e.what() << endl;
		exit(E_RUNTIME_ERROR);
	}
	
	if ( ldapMessage_rec.MessageType == LDAPMessageType_t::SearchRequest ) {
		ldapMessage_send.MessageID = ldapMessage_rec.MessageID;
		ldapMessage_send.MessageType = LDAPMessageType_t::SearchResultEntry;
		
		ldapMessage_send.SearchResultEntry.ObjectName = "uid=xberky02";
		
		PartialAttributeList_t attr;
		attr.Type = "cn"; attr.Values.emplace_back("Levente Berky");
		ldapMessage_send.SearchResultEntry.Attributes.push_back(attr);
		
		attr.Values.clear();
		attr.Type = "uid"; attr.Values.emplace_back("xberky02");
		ldapMessage_send.SearchResultEntry.Attributes.push_back(attr);
		msg = encoder.struct_to_hex(ldapMessage_send);
		con.send_message(msg);
		
		ldapMessage_send.MessageType = LDAPMessageType_t::SearchResultDone;
		ldapMessage_send.LDAPResult.ResultCode = decoder.get_error().get_code();
		ldapMessage_send.LDAPResult.MatchedDN = "";
		ldapMessage_send.LDAPResult.ErrorMessage = decoder.get_error().get_message();
		msg = encoder.struct_to_hex(ldapMessage_send);
		con.send_message(msg);
	}
	
	// UnbindRequest
	try {
		msg = con.get_message();
		ldapMessage_rec = decoder.decode_to_struct(msg);
	} catch (runtime_error &e) {
		cerr << "Runtime Error occurred: " << e.what() << endl;
		exit(E_RUNTIME_ERROR);
	}
	
	if ( ldapMessage_rec.MessageType != UnbindRequest ) {
		cerr << "Expected UnbindRequest, got different" << endl;
		exit(E_RUNTIME_ERROR);
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