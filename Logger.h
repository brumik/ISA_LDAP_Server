//
// Created by levente on 11/5/17.
//

#ifndef ISA_LDAP_SERVER_LOGGER_H
#define ISA_LDAP_SERVER_LOGGER_H

#include "Codes.h"
#include <string>

class Logger {
private:
	
	/**
	 * Prints substrings.
	 *
	 * @param substrings The substrings to print.
	 * @return Stringified substrings.
	 */
	static std::string print_substrings(const Substrings_t &substrings);
	
	/**
	 * Prints filter.
	 *
	 * @param filter The filter to print.
	 * @return Stringified filter.
	 */
	static std::string print_filter(const Filter_t &filter);
	
	/**
	 * Prints search request.
	 *
	 * @param request The search request to print.
	 * @return Stringified search request.
	 */
	static std::string print_search_request(const SearchRequest_t &request);

public:
	
	/**
	 * Prints the LDAP message.
	 *
	 * @param ldapMessage The message to print.
	 * @return Stringified LDAP message.
	 */
	static std::string print_ldap_message(const LDAPMessage_t &ldapMessage);
};


#endif //ISA_LDAP_SERVER_LOGGER_H
