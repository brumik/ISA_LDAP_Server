/**
 * @file LDAPCodes.h
 * @author Levente Berky
 *
 * This file contains all constants for LDAP Server
 */


#ifndef ISA_LDAP_SERVER_LDAPCODES_H
#define ISA_LDAP_SERVER_LDAPCODES_H


struct BindRequest {
	unsigned long Version = 0;
	std::string Name = "";
	std::string Auth = "";
};

struct BindResponse {
	unsigned long ResultCode = 0;
	std::string MatchedDN = "";
	std::string ErrorMessage = "";
};

struct LDAPMessage {
	unsigned long MessageID = 0;
	std::string MessageType = "00";
	struct BindRequest BindRequest;
	struct BindResponse BindResponse;
};


class Codes {
public:
	static constexpr const char* INTEGER = "02";
	static constexpr const char* OCTET_STRING = "04";
	
	// Structure types
	static constexpr const char* LDAPMessage = "30";
	static constexpr const char* BindRequest = "60";
	static constexpr const char* BindResponse = "61";
	static constexpr const char* SIMPLE_AUTH = "8000";
};


#endif