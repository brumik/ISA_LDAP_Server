/**
 * @file Encode.h
 * @author Levente Berky
 *
 * Used to encode LDAP structures into hex string.
 */

#ifndef ISA_LDAP_SERVER_ENCODE_H
#define ISA_LDAP_SERVER_ENCODE_H

#include "Codes.h"
#include <string>

class Encode {
private:
	/**
	 * Converts the given number to hex string.
	 *
	 * @param number The number to convert to hex string.
	 * @return The converted hex number.
	 */
	std::string number_to_hex(unsigned long number);
	
	/**
	 * Converts the givens size number to hex. Basicly same as number_to_hex just the number is divided by two.
	 *
	 * @param size The size in bits.
	 * @return The hex length of the given number
	 */
	std::string size_to_hex(unsigned long size);
	
	/**
	 * Converts number with type, size, value hex.
	 *
	 * @param number The number to convert.
	 * @return The hex string generated from number.
	 */
	std::string create_number(unsigned long number);
	
	/**
	 * Converts string to hex.
	 *
	 * @param str The string to convert.
	 * @return Hex coded string.
	 */
	std::string string_to_hex(const std::string &str);
	
	/**
	 * Converts octet string with type, size, value hex.
	 *
	 * @param str The string to convert.
	 * @return The hex string for the given octet string.
	 */
	std::string create_string(const std::string &str);
	
	/**
	 * Converts BindResponse structure to hex.
	 *
	 * @param bindResponse The structure to convert.
	 * @return The converted BindResponse structure in hex string.
	 */
	std::string bind_response_to_hex(const struct BindResponse &bindResponse);
	
public:
	
	/**
	 * Generates encoded hex string from the given LDAPMessage structure.
	 *
	 * @param ldapMessage The structure to convert to hex.
	 * @return HEX encoded LDAPMessage.
	 */
	std::string structure_to_hex(const LDAPMessage &ldapMessage);
};


#endif //ISA_LDAP_SERVER_ENCODE_H
