/**
 * @file ResponseBuilder.h
 * @author Levente Berky
 *
 * This class builds response to client messages like BndRequest or SearchRequest.
 */

#ifndef ISA_LDAP_SERVER_RESPONSEBUILDER_H
#define ISA_LDAP_SERVER_RESPONSEBUILDER_H

#include <vector>
#include "Database.h"
#include "Codes.h"
#include "Error.h"

class ResponseBuilder {
private:
	std::string matchedDN;
	Database db;
	
	/**
	 * Generates an BindResponse to an BindRequest.
	 *
	 * @param error The result to generate respnd from.
	 * @return LDAP Result structure.
	 */
	LDAPResult_t create_result(Error &error);
	
	/**
	 * Using Database operations select and returns proper entries for the request.
	 *
	 * @param substring The substring filter to filter the entries.
	 * @return The database filled with result entries.
	 */
	Database request_filter_substring(const Substrings_t &substrings);
	
	/**
	 * Using Database operations select and returns proper entries for the request.
	 *
	 * @param equality The equality match filter to filter the entries.
	 * @return The database filled with result entries.
	 */
	Database request_filter_equality(const AttributeValueAssertion_t &equality);
	
	/**
	 * Using Database operations select and returns proper entries for the request.
	 *
	 * @param filter The filter to filter the entries.
	 * @return The database filled with result entries.
	 */
	Database request_filter(const Filter_t &filter);
	
public:
	/**
	 * Default constructor.
	 */
	explicit ResponseBuilder(Database &database);
	
	/**
	 * Generates a response to a request.
	 *
	 * @param request The request to generate response.
	 * @return LDAP Message ready to send.
	 */
	std::vector<LDAPMessage_t> generate_entries_response(const LDAPMessage_t &request);
	
	/**
	 * Generates response based on error message.
	 *
	 * @param request The request to generate response.
	 * @param error The error message containing error code and message.
	 * @return LDAP Message ready to send.
	 */
	LDAPMessage_t generate_response(const LDAPMessage_t &request, Error error);
};


#endif //ISA_LDAP_SERVER_RESPONSEBUILDER_H
