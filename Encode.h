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
	 * Covert number with type, size, value to enumerated hex.
	 *
	 * @param number The value of enumerate.
	 * @return The hex string generated from enumerated number.
	 */
	std::string create_enum(unsigned long number);
	
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
	 * Creates a partial attribute hex string.
	 *
	 * @param attr The attribute to create string from.
	 * @return Hex string representing the attribute.
	 */
	std::string create_partial_attribute(const PartialAttributeList_t &attribute);
	
	/**
	 * Converts LDAP Result structure to hex.
	 *
	 * @param result The structure to convert.
	 * @return The converted Result structure in hex string.
	 */
	std::string struct_to_hex(const LDAPResult_t &result, const std::string &code);
	
	/**
	 * Converts an SearchEntry structure to hex.
	 *
	 * @param entry The entry to convert.
	 * @return The converted SearchEntry structure in hex string.
	 */
	std::string struct_to_hex(const SearchResultEntry_t &entry);

public:
	
	/**
	 * Generates encoded hex string from the given LDAPMessage structure.
	 *
	 * @param ldapMessage The structure to convert to hex.
	 * @return HEX encoded LDAPMessage.
	 */
	std::string struct_to_hex(const LDAPMessage_t &ldapMessage);
};


#endif //ISA_LDAP_SERVER_ENCODE_H
