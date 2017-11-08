#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"
#include "Decode.h"
#include "Encode.h"
#include "ResponseBuilder.h"

#define EXIT_WRONG_ARGS     1
#define EXIT_RUNTIME_ERROR  2

using namespace std;

// The database all process will use.
Database db;

void process_request(Connection con)
{
	ResponseBuilder builder(db);
	
	Decode decoder;
	Encode encoder;
	LDAPMessage_t request, message;
	string hex_message;
	
	try {
		while (true) {
			// Real work
			hex_message = con.get_message();
			request = decoder.decode_to_struct(hex_message);
			
			// Creating and sending response
			if ( request.MessageType == LDAPMessageType_t::BindRequest ) {
				message = builder.generate_response(request, decoder.get_error());
				hex_message = encoder.struct_to_hex(message);
				con.send_message(hex_message);
				
			} else if ( request.MessageType == LDAPMessageType_t::UnbindRequest ) {
				break;
				
			} else if ( request.MessageType == LDAPMessageType_t::SearchRequest ) {
				
				for( auto &entryResponse : builder.generate_entries_response(request)) {
					hex_message = encoder.struct_to_hex(entryResponse);
					con.send_message(hex_message);
				}
				
				message = builder.generate_response(request, decoder.get_error());
				hex_message = encoder.struct_to_hex(message);
				con.send_message(hex_message);
				
			} else {
				decoder.get_error().set_error(ResultCode_e::PROTOCOL_ERROR, "Request not supported");
				throw runtime_error(decoder.get_error().get_message());
			}
		}
		
	} catch (const runtime_error &e) {
		// Set te error request type
		request.MessageType = decoder.get_type();
		
		// Send the error message.
		message = builder.generate_response(request, decoder.get_error());
		hex_message = encoder.struct_to_hex(message);
		con.send_message(hex_message);
		
		cerr << "Runtime Error occurred: " << e.what() << endl;
		exit(EXIT_RUNTIME_ERROR);
	}
	
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	// Argument processing
	long port = 389;
    string file;
    int c;
    while ( (c = getopt (argc, argv, "f:p:")) != -1 ) {
	    switch ( c ) {
		    case 'f':
			    file = optarg;
			    break;
		    case 'p':
			    port = strtol(optarg, nullptr, 10);
			    break;
		    case '?':
			    cerr << "Invalid arguments." << endl;
			    return EXIT_WRONG_ARGS;
		    default:
			    return EXIT_WRONG_ARGS;
	    }
    }
	
	if ( file.empty() ) {
		cerr << "File not specified. Use -f <filename>" << endl;
		return EXIT_WRONG_ARGS;
	}
	
	// Creating the connection and trying to create the child process.
	// If cannot create the child process ends with the appropriate message.
	Connection con(port);
	try {
		db.open(file);
		con.server_up();
		con.accept_loop(process_request);
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred before subprocess: " << e.what() << endl;
	} catch (...) {
		cerr << "Something happened." << endl;
	}
	
    return 0;
}