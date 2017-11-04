/**
 * @file Encode.h
 * @author Levente Berky
 *
 * Used to encode LDAP structures into hex string.
 */

#include <ios>
#include <iomanip>
#include <sstream>
#include "Encode.h"

using namespace std;

std::string Encode::number_to_hex(unsigned long number)
{
	std::stringstream stream;
	stream << setfill('0') << setw(2) << hex << number;
	return stream.str();
}

string Encode::size_to_hex(unsigned long size)
{
	return number_to_hex(size / 2);
}

string Encode::create_number(unsigned long number)
{
	string hex_number = number_to_hex(number);
	string hex_size = size_to_hex(hex_number.length());
	
	return string(Codes::INTEGER) + hex_size + hex_number;
}

std::string Encode::create_enum(unsigned long number)
{
	string hex_number = number_to_hex(number);
	string hex_size = size_to_hex(hex_number.length());
	
	return string(Codes::ENUMERATED) + hex_size + hex_number;
}

string Encode::string_to_hex(const string &str)
{
	ostringstream ret;
	for (auto c : str)
		ret << hex << setfill('0') << setw(2) << uppercase << (int)c;
	return ret.str();
}

string Encode::create_string(const std::string &str)
{
	string val = string_to_hex(str);
	
	string ret = Codes::OCTET_STRING;
	ret += size_to_hex( val.length() );
	ret += val;
	
	return ret;
}

string Encode::bind_response_to_hex(const struct BindResponse_t &bindResponse)
{
	string resultCode = create_enum(static_cast<unsigned>(bindResponse.ResultCode));
	string MatchedDN = create_string(bindResponse.MatchedDN);
	string ErrorMessage = create_string(bindResponse.ErrorMessage);
	
	string ret = Codes::BindResponse;
	ret += size_to_hex( resultCode.length() + MatchedDN.length() + ErrorMessage.length() );
	ret += resultCode + MatchedDN + ErrorMessage;
	return ret;
}

string Encode::structure_to_hex(const LDAPMessage_t &ldapMessage)
{
	string messageStructure;
	string messageID;
	string ret;
	
	messageID = create_number(ldapMessage.MessageID);
	
	if ( ldapMessage.MessageType == LDAPMessageType_t::BindResponse ) {
		messageStructure = bind_response_to_hex(ldapMessage.BindResponse);
	} else {
		throw runtime_error("Unrecognized message type to create.");
	}
	
	
	ret = Codes::LDAPMessage;
	ret += size_to_hex( messageID.length() + messageStructure.length() );
	ret += messageID;
	ret += messageStructure;
	
	return ret;
}
