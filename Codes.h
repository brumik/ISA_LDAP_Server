/**
 * @file LDAPCodes.h
 * @author Levente Berky
 *
 * This file contains all constants for LDAP Server
 */


#ifndef ISA_LDAP_SERVER_LDAPCODES_H
#define ISA_LDAP_SERVER_LDAPCODES_H

#include <vector>

struct BindRequest_t {
	unsigned long Version = 0;
	std::string Name = "";
	std::string Auth = "";
};

/**
 * @defgroup BindResponse
 * This group contains all BindResponse structures
 * @{
 */

enum ResultCode_e {
	SUCCESS,
	OPERATIONS_ERROR,
	PROTOCOL_ERROR,
	TIME_LIMIT_EXCEED,
	SIZE_LIMIT_EXCEED,
	COMPARE_FALSE,
	COMPARE_TRUE,
	AUTH_NOT_SUPPORTED,
};

struct BindResponse_t {
	ResultCode_e ResultCode = ResultCode_e::SUCCESS;
	std::string MatchedDN = "";
	std::string ErrorMessage = "";
};

/** @} */ // End of BindResponse

/**
 * @defgroup SearchRequest
 * This group contains all SearchRequest structures
 * @{
 */

struct AttributeValueAssertion_t {
	std::string AttributeDesc = "";
	std::string AssertionValue = "";
};

enum Substring_Item_Type_e { Initial, Any, Final };
struct Substring_Item_t {
	Substring_Item_Type_e Type = Substring_Item_Type_e::Any;
	std::string data = "";
};

struct Substrings_t {
	std::string Type = "";
	std::vector<Substring_Item_t> items;
};

enum FilterType_e { And, Or, Not, EqualityMatch, Substrings };
struct Filter_t {
	FilterType_e Type = FilterType_e::EqualityMatch;
	std::vector<Filter_t> And;
	std::vector<Filter_t> Or;
	Filter_t *Not = nullptr;
	AttributeValueAssertion_t EqualityMatch;
	Substrings_t Substrings;
};


enum Scope_e { BaseObject, SingleLevel, WholeSubTree };
enum DerefAliases_e { NeverDerefAliases, DerefInSearching, DerefFindingBaseObject, DerefAlways };
struct SearchRequest_t {
	std::string BaseObject = "";
	Scope_e Scope = Scope_e::BaseObject;
	DerefAliases_e DerefAliases = DerefAliases_e::NeverDerefAliases;
	unsigned long SizeLimit = 0;
	unsigned long TimeLimit = 0;
	bool TypesOnly = false;
	Filter_t Filter;
	std::vector<std::string> Attributes = {};
};
/** @} */ // End of SearchRequest_t

enum LDAPMessageType_t {
	BindRequest,
	BindResponse,
	SearchRequest,
};

struct LDAPMessage_t {
	unsigned long MessageID = 0;
	LDAPMessageType_t MessageType = LDAPMessageType_t::BindRequest;
	struct BindRequest_t BindRequest;
	struct BindResponse_t BindResponse;
	struct SearchRequest_t SearchRequest;
};


class Codes {
public:
	static constexpr const char* BOOLEAN = "01";
	static constexpr const char* INTEGER = "02";
	static constexpr const char* OCTET_STRING = "04";
	static constexpr const char* ENUMERATED = "0a";
	static constexpr const char* SIMPLE_AUTH = "8000";
	
	// Structure types
	static constexpr const char* LDAPMessage = "30";
	static constexpr const char* BindRequest = "60";
	static constexpr const char* BindResponse = "61";
	static constexpr const char* SearchRequest = "63";
	
	// Filter
	static constexpr const char* FilterAnd = "a0";
	static constexpr const char* FilterOr = "a1";
	static constexpr const char* FilterNot = "a2";
	static constexpr const char* FilterSubstring = "a4";
	static constexpr const char* FilterEqualityMatch = "a3";

	// Filter Substring Item Types
	static constexpr const char* FilterSubstringItemInitial = "80";
	static constexpr const char* FilterSubstringItemAny = "81";
	static constexpr const char* FilterSubstringItemFinal = "82";
};


#endif