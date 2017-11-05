/**
 * @file Decode.h
 * @author Levente Berky
 *
 * TODO
 */

#ifndef ISA_LDAP_SERVER_DECODE_H
#define ISA_LDAP_SERVER_DECODE_H

#include <string>
#include "Codes.h"
#include "Error.h"

class Decode {
private:
	std::string hex_message;
	unsigned int position;
	Error error;
	
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
	 * Returns the length of something in chars length from hex_message.
	 * @return
	 */
	unsigned long get_size_in_char();
	
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
	 * Returns the next boolean value grom hex_message.
	 *
	 * @return True or false from the HEX string.
	 */
	bool get_bool();
	
	/**
	 * Converts HEX encoded string to ASCII.
	 *
	 * @param hex Hex encoded string.
	 * @return The ASCII string.
	 */
	std::string hex_to_string(const std::string &hex);
	
	/**
	 * Gets a string from the hex string.
	 *
	 * @return String containing ASCII values of HEX numbers.
	 */
	std::string get_string();
	
	/**
	 * Gets an enum from the hex_message.
	 *
	 * @return The enum number.
	 */
	unsigned long get_enum();
	
	/**
	 * Returns the integer from the HEX string.
	 *
	 * @return Integer specified in HEX string.
	 */
	unsigned long get_messageID();
	
	/**
	 * Fills and returns an bind request from the hex_message.
	 *
	 * @return The bind request structure filled.
	 */
	BindRequest_t get_bindRequest();
	
	/**
	 * Fills and returns an Substring Item from the hex_message.
	 *
	 * @return The Substring Item structure filled.
	 */
	Substring_Item_t get_substring_item();
	
	/**
	 * Fills and returns a Substring from the hex_message.
	 *
	 * @return Filled substring.
	 */
	Substrings_t get_substrings();
	
	/**
	 * Fills and returns a Filter structure from the hex_message.
	 *
	 * @return Filled filter.
	 */
	Filter_t get_filter();
	
	/**
	 * Fills and return a search request structure from the hex_message.
	 *
	 * @return The search request structure filled.
	 */
	SearchRequest_t get_searchRequest();
	
public:
	
	/**
	 * Constructor.
	 */
	Decode();
	
	/**
	 * Constructor.
	 * @param to_decode The string to decode.
	 */
	LDAPMessage_t decode_to_struct(std::string &to_decode);
	
	/**
	 * Error getter.
	 *
	 * @return The error object.
	 */
	Error get_error();
	
};

#endif //ISA_LDAP_SERVER_DECODE_H
