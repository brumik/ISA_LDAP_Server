/**
 * @file Errors.h
 * @author Levente Berky
 *
 * Object for errors.
 */
#ifndef ISA_LDAP_SERVER_ERRORS_H
#define ISA_LDAP_SERVER_ERRORS_H

#include <string>

class Error {
private:
	unsigned code;
	std::string message;
	
public:
	
	// Error codes
	static const unsigned SUCCESS = 0;
	static const unsigned OPERATIONS_ERROR = 1;
	static const unsigned PROTOCOL_ERROR = 2;
	static const unsigned TIME_LIMIT_EXCEED = 3;
	static const unsigned SIZE_LIMIT_EXCEED = 4;
	static const unsigned AUTH_NOT_SUPPORTED = 7;

	/**
	 * Constructor for error.
	 */
	Error();
	
	/**
	 * Sets the error to the given code and the given message.
	 *
	 * @param code The error code.
	 * @param message The error message.
	 */
	void set_error(unsigned code, const std::string &message);
	
	/**
	 * Code getter.
	 *
	 * @return COde attribute value.
	 */
	unsigned get_code();
	
	/**
	 * Message getter.
	 *
	 * @return Message attribute value.
	 */
	std::string get_message();
	
	/**
	 * Clears the message and sets the error code to 0.
	 */
	void clear();
};


#endif //ISA_LDAP_SERVER_ERRORS_H
