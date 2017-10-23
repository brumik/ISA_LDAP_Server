#include <iostream>
#include <getopt.h>
#include "Connection.h"
#include "Database.h"

#define E_WRONG_ARGS 1

/**
 * Global variable for database. Used in all children process but read in only once.
 */
database_t db;

using namespace std;

void process_request(Connection con) {
    cout << "New client connected" << endl;
	try {
		string msg = con.getMessage();
		cout << "Message is: " << msg << endl;
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred:" << e.what() << endl;
	}
	cout << "Klient serving ended." << endl;
}

int main(int argc, char *argv[]) {
    int port = 389;
    string file;
    int c;
    while ((c = getopt (argc, argv, "f:p:")) != -1)
        switch (c) {
            case 'f':
                file = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                cerr << "Invalid arguments." << endl;
                return E_WRONG_ARGS;
            default:
                return E_WRONG_ARGS;
        }

    Connection con(port);
	try {
		con.serverUp();
		db = Database::get_db_from_csv(file);
		con.acceptLoop(process_request);
	} catch (const runtime_error &e) {
		cerr << "Runtime Error occurred before subprocess: " << e.what() << endl;
	} catch (...) {
		cerr << "Something happened." << endl;
	}
	
    return 0;
}