#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"
#include "Decode.h"
#include "Encode.h"
#include "ResponseBuilder.h"

/* Not used in this moment.
 * #include "Logger.h"
 */

#define EXIT_WRONG_ARGS     1
#define EXIT_RUNTIME_ERROR  2

using namespace std;

void process_request(Connection con)
{
	// Variables used
	Database db(con.get_filename());
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
				
			}
		}
		
	} catch (const runtime_error &e) {
		
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
//	Database db(argv[4]);
//	vector<string> v;
//	cout << db.filter_by_uid_substrings("xbe", v, "02").toString();
//	return 0;
	
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
                return EXIT_WRONG_ARGS;
            default:
                return EXIT_WRONG_ARGS;
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