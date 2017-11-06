/**
 * @file Connection.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This file contains the definitions of the connection management.
 * Listening, binding, sending and receiving data is done trough these functions.
 */

#ifndef ISA_LDAP_SERVER_CONNECTION_H
#define ISA_LDAP_SERVER_CONNECTION_H

#include <string>
#include <vector>

const int BUFFER_SIZE = 512;

class Connection {

private:
    long port;
	std::string filename;
	
    int server_sock;
    int socket_desc;

public:
    /**
     * Constructor for server.
     *
     * @param port The port to run the server
     * @param filename The path to the database csv.
     */
    Connection(long port, const std::string& filename);
	
	/**
	 * Filename getter.
	 *
	 * @return Filename
	 */
	std::string get_filename();

    /**
     * Prepares the server to listen and starts to listen.
     *
     * @return Return true if succeeded otherwise false.
     */
    void server_up();

    /**
     * Starts a bind loop to accepting incoming requests.
     * Then passes to the newly forked process.
     *
     * @param callback Function to pass to a newly forked process.
     */
    void accept_loop(void (*callback)(Connection));
	
    /**
     * Returns the data as string sent to the server.
     * Throws exception on bad data transfer,
     *
     * @return String containing message received.
     */
    std::string get_message();
	
	/**
	 * Converts hex string to bytes.
	 * @param hex The hex string.
	 * @return Bytes vector.
	 */
	std::vector<unsigned char> hex_to_bytes(const std::string& hex);
	
	/**
	 * Sends the given string to the connected client.
	 *
	 * @return True on success, otherwise false.
	 */
    void send_message(const std::string &msg);

    /**
     * Destructor, closes all sockets.
     */
    ~Connection();
};


#endif //ISA_LDAP_SERVER_CONNECTION_H
