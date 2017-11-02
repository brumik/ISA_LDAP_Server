/**
 * @file Decode.h
 * @author Levente Berky
 *
 * TODO
 */

#ifndef ISA_LDAP_SERVER_DECODE_H
#define ISA_LDAP_SERVER_DECODE_H

#include <string>

struct BindRequest {
	unsigned long Version = 0;
	std::string Name;
	std::string Auth;
};

struct LDAPMessage {
	unsigned long MessageID = 0;
	struct BindRequest BindRequest;
};

class Decode {
private:
	const char* C_LDAPMessage = "30";
	const char* C_BindRequest = "60";
	const char* C_INTEGER = "02";
	const char* C_OCTET_STRING = "04";
	const char* C_SIMPLE_AUTH = "8000";

	std::string hex_message;
	unsigned int position;
	
	/**
	 * Returns the next two characters from string and deletes them from the hex message.
	 *
	 * @param num Number of hex twins to get.
	 * @return Two * num character length hex string.
	 */
	std::string get_next_hex_string(unsigned long num = 1);
	
	/**
	 * Returns the length of something.
	 *
	 * @return The length of the next data.
	 */
	unsigned long get_size();
	
	/**
	 * Controls if the specified length is same as the remaining message length.
	 *
	 * @return True is lentgh and remaining message length is same. Otherwise false.
	 */
	bool size_control();
	
	/**
	 * Returns the integer.
	 *
	 * @param max_length Max length of hex string.
	 * @return Unsigned integer.
	 */
	unsigned long get_integer(int max_length = 255);
	
	/**
	 * Gets a string from the hex string.
	 *
	 * @return String containing ASCI values of HEX numbers.
	 */
	std::string get_string();
	
	/**
	 * Returns the integer from the HEX string.
	 *
	 * @return Integer specified in HEX string.
	 */
	unsigned long get_messageID();
	
	/**
	 * Fils and returns an bind request from the hex_message.
	 * @return The bind request structure.
	 */
	BindRequest get_bindRequest();
	
public:
	
	/**
	 * Constructor.
	 */
	Decode();
	
	/**
	 * Constructor.
	 * @param to_decode The string to decode.
	 */
	LDAPMessage decode_to_struct(std::string &to_decode);
	
};

#endif //ISA_LDAP_SERVER_DECODE_H