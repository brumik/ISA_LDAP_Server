/**
 * @file Errors.h
 * @author Levente Berky
 *
 * Object for errors.
 */
#ifndef ISA_LDAP_SERVER_ERRORS_H
#define ISA_LDAP_SERVER_ERRORS_H

#include <string>
#include "Codes.h"

class Error {
private:
	ResultCode_e code;
	std::string message;
	
public:
	/**
	 * Constructor for error.
	 */
	Error();
	
	/**
	 * Constructor for error.
	 *
	 * @param code Error code.
	 * @param message Error message.
	 */
	Error(unsigned code, const std::string &message);
	
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
	ResultCode_e get_code();
	
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
