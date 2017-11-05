/**
 * @file Decode.cpp
 * @author Levente Berky
 *
 * TODO
 */

#include <sstream>
#include <iostream>
#include "Decode.h"

using namespace std;

Decode::Decode()
{
	hex_message = "";
	position = 0;
	error.clear();
}

string Decode::get_next_hex_string(unsigned long num)
{
	if ((position + num * 2) > hex_message.length())
		throw runtime_error("Too much of requested number of hex pair.");
	
	string ret = hex_message.substr(position, num * 2);
	position += num * 2;
	return ret;
}

unsigned long Decode::get_size()
{
	string size = get_next_hex_string();
	
	if ( size == Codes::LengthSize1 )
		size = get_next_hex_string(1);
	else if ( size == Codes::LengthSize2 )
		size = get_next_hex_string(2);
	else if ( size == Codes::LengthSize3 )
		size = get_next_hex_string(3);
	else if ( size == Codes::LengthSize4 )
		size = get_next_hex_string(4);
		
	return stoul(size, nullptr, 16);
}

unsigned long Decode::get_size_in_char()
{
	return get_size() * 2;
}

bool Decode::size_control()
{
	return get_size() == (hex_message.length() - position) / 2;
}

unsigned long Decode::get_integer(int max_length)
{
	if ( get_next_hex_string() != Codes::INTEGER )
		throw runtime_error("Integer is not defined.");
	
	unsigned long size = get_size();
	if ( size > max_length )
		throw length_error("Integer is too long");
	
	return stoul(get_next_hex_string(size), nullptr, 16);
}

bool Decode::get_bool()
{
	if ( get_next_hex_string() != Codes::BOOLEAN )
		throw runtime_error("Boolean is not defined.");
	
	if ( get_size() != 1 )
		throw length_error("Boolean is too long");
	
	return get_next_hex_string() == "01";
}

string Decode::hex_to_string(const std::string &hex)
{
	string ret;
	
	for( unsigned long i = 0; i < hex.size(); i += 2 )
		ret.push_back((char) strtol(hex.substr(i, 2).c_str(), nullptr, 16));
	
	return ret;
}

string Decode::get_string()
{
	if ( get_next_hex_string() != Codes::OCTET_STRING )
		throw runtime_error("Not string.");
	
	return hex_to_string( get_next_hex_string( get_size() ) );
}

unsigned long Decode::get_enum()
{
	if ( get_next_hex_string() != Codes::ENUMERATED )
		throw runtime_error("Enumerated is not defined.");
	
	return stoul(get_next_hex_string( get_size() ), nullptr, 16);
}

unsigned long Decode::get_messageID()
{
	try {
		return get_integer(4);
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "MessageID is missing.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "MessageID is too long.");
		throw runtime_error(error.get_message());
	}
}

BindRequest_t Decode::get_bindRequest()
{
	BindRequest_t request;
	
	// BindRequest_t size
	if ( !size_control() ) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "BindRequest length has different size than specified.");
		throw runtime_error(error.get_message());
	}
	
	// Version of client
	try {
		request.Version = get_integer(1);
		
		// If version is not 2 or 3 then it is not supported.
		if ( !(request.Version == 3 || request.Version == 2) ) {
			error.set_error(ResultCode_e::PROTOCOL_ERROR, "Protocol version is not supported.");
			throw runtime_error(error.get_message());
		}
		
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "BindRequest Version is not specified.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "BindRequest Version number has too long size.");
		throw runtime_error(error.get_message());
	}
	
	// Name of non anonymous client.
	try {
		request.Name = get_string();
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, string("Getting Bind Name encountered an error: ") + e.what());
		throw runtime_error(error.get_message());
	}
	
	// Auth type of client.
	if ( get_next_hex_string(2) != Codes::SIMPLE_AUTH ) {
		error.set_error(ResultCode_e::AUTH_NOT_SUPPORTED, "Not simple auth whiteout password.");
		throw runtime_error(error.get_message());
	}
	
	return request;
}

Substring_Item_t Decode::get_substring_item()
{
	Substring_Item_t item;
	
	string type = get_next_hex_string();
	if ( type == Codes::FilterSubstringItemInitial )
		item.Type = Substring_Item_Type_e::Initial;
	else if ( type == Codes::FilterSubstringItemAny )
		item.Type = Substring_Item_Type_e::Any;
	else if ( type == Codes::FilterSubstringItemFinal )
		item.Type = Substring_Item_Type_e::Final;
	else {
		throw runtime_error("Invalid Filter Substring Item type.");
	}
	
	item.data = hex_to_string( get_next_hex_string( get_size() ) );
	
	return item;
}

Substrings_t Decode::get_substrings()
{
	Substrings_t substrings;
	
	unsigned long size = get_size_in_char();
	unsigned long end_position = position + size;
	
	try {
		substrings.Type = get_string();
	} catch (runtime_error &e) {
		throw runtime_error("Substring type is not properly defined.");
	}
	
	try {
		if ( get_next_hex_string() != Codes::FilterSubstringItem )
			throw runtime_error("Substring item not starting with sequence.");
		
		if ( get_size_in_char()  != end_position - position )
			throw runtime_error("The substring length is inproper.");
		
		// While size specified read... they will be all substrings
		while ( end_position > position )
			substrings.items.push_back( get_substring_item() );
	} catch (runtime_error &e) {
		throw runtime_error(string("Substring Items: ") + e.what());
	}
	
	return substrings;
}

Filter_t Decode::get_filter()
{
	Filter_t filter;
	
	string type = get_next_hex_string();
	
	if ( type == Codes::FilterAnd ) {
		filter.Type = FilterType_e::And;
		// Todo And
	} else if ( type == Codes::FilterOr ) {
		filter.Type = FilterType_e::Or;
		// Todo Or
	} else if ( type == Codes::FilterNot ) {
		filter.Type = FilterType_e::Not;
		// Todo Not
	} else if ( type == Codes::FilterSubstring ) {
		filter.Type = FilterType_e::Substrings;
		filter.Substrings = get_substrings();
	} else if ( type == Codes::FilterEqualityMatch ) {
		filter.Type = FilterType_e::EqualityMatch;
		get_size(); // Todo What to do with size?
		filter.EqualityMatch.AttributeDesc = get_string();
		filter.EqualityMatch.AssertionValue = get_string();
	} else {
		throw runtime_error("Invalid Filter type.");
	}
	
	return filter;
}

SearchRequest_t Decode::get_searchRequest()
{
	SearchRequest_t request;
	
	if ( !size_control() ) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest length has different size than specified.");
		throw runtime_error(error.get_message());
	}
	
	// BaseObject
	try {
		request.BaseObject = get_string();
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest BaseObject string is not valid.");
		throw runtime_error(error.get_message());
	}
	
	// Scope
	try {
		request.Scope = static_cast<Scope_e>(get_enum());
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest Scope is not valid.");
		throw runtime_error(error.get_message());
	}
	
	// DerefAliases
	try {
		request.DerefAliases = static_cast<DerefAliases_e >(get_enum());
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest DerefAliases is not valid.");
		throw runtime_error(error.get_message());
	}
	
	// SizeLimit
	try {
		request.SizeLimit = get_integer();
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest SizeLimit is not specified.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest SizeLimit number has too long size.");
		throw runtime_error(error.get_message());
	}
	
	// TimeLimit
	try {
		request.TimeLimit = get_integer();
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest TimeLimit is not specified.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest TimeLimit number has too long size.");
		throw runtime_error(error.get_message());
	}
	
	// TypesOnly
	try {
		request.TypesOnly = get_bool();
	} catch (runtime_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest TypesOnly is not specified.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "SearchRequest TypesOnly boolean has too long size.");
		throw runtime_error(error.get_message());
	}
	
	// Filter
	try {
		request.Filter = get_filter();
	} catch (...) {
		// Todo Error reporting.
		throw;
	}
	
	return request;
}

LDAPMessage_t Decode::decode_to_struct(std::string &to_decode)
{
	// Reset error message.
	error.clear();
	hex_message = to_decode;
	position = 0;
	LDAPMessage_t message;
	
	
	if ( get_next_hex_string() != Codes::LDAPMessage ) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "Message is non LDAP Message.");
		throw runtime_error(error.get_message());
	}
	
	if ( !size_control() ) {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "LDAPMessage_t has different size than specified.");
		throw runtime_error(error.get_message());
	}
	
	message.MessageID = get_messageID();
	
	// Determine what kind of message we got and do the appropriate action.
	string type = get_next_hex_string();
	
	if ( type == Codes::BindRequest ) {
		message.MessageType = LDAPMessageType_t::BindRequest;
		message.BindRequest = get_bindRequest();
	} else if ( type == Codes::SearchRequest ) {
		message.MessageType = LDAPMessageType_t::SearchRequest;
		message.SearchRequest = get_searchRequest();
	} else if ( type == Codes::UnbindRequest) {
		message.MessageType = LDAPMessageType_t::UnbindRequest;
	} else {
		error.set_error(ResultCode_e::PROTOCOL_ERROR, "Unrecognized message type.");
		throw runtime_error(error.get_message());
	}
	
	return message;
}

Error Decode::get_error()
{
	return error;
}
