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
	string number = number_to_hex(size / 2);
	
	switch( number.length() / 2 ) {
		case 1: return number;
		case 2: return Codes::LengthSize2 + number;
		case 3: return Codes::LengthSize3 + number;
		case 4: return Codes::LengthSize4 + number;
		default: throw runtime_error("Was unable to convert size to hex.");
	}
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

string Encode::create_partial_attribute(const PartialAttributeList_t &attribute)
{
	string type = create_string(attribute.Type);
	
	// Values
	string valueStrings;
	for (auto &value : attribute.Values)
		valueStrings += create_string(value);
	
	string values = Codes::ResponseEntryPartialAttributeValues;
	values += size_to_hex(valueStrings.length());
	values += valueStrings;
	// End Values
	
	string ret = Codes::ResponseEntryPartialAttribute;
	ret += size_to_hex( type.length() + values.length() );
	ret += type + values;
	return ret;
}

string Encode::struct_to_hex(const LDAPResult_t &result, const std::string &code)
{
	string resultCode = create_enum(static_cast<unsigned>(result.ResultCode));
	string MatchedDN = create_string(result.MatchedDN);
	string ErrorMessage = create_string(result.ErrorMessage);
	
	string ret = code;
	ret += size_to_hex( resultCode.length() + MatchedDN.length() + ErrorMessage.length() );
	ret += resultCode + MatchedDN + ErrorMessage;
	return ret;
}

string Encode::struct_to_hex(const SearchResultEntry_t &entry)
{
	string ObjectName = create_string(entry.ObjectName);
	
	// Creating Attributes
	string AttributeValues;
	for (const PartialAttributeList_t &attr : entry.Attributes)
		AttributeValues += create_partial_attribute(attr);
	
	string Attributes = Codes::ResponseEntryAttributes;
	Attributes += size_to_hex( AttributeValues.length() );
	Attributes += AttributeValues;
	
	// Creating return string
	string ret = Codes::SearchResultEntry;
	ret += size_to_hex( ObjectName.length() + Attributes.length() );
	ret += ObjectName + Attributes;
	return ret;
}

string Encode::struct_to_hex(const LDAPMessage_t &ldapMessage)
{
	string messageStructure;
	string messageID;
	string ret;
	
	messageID = create_number(ldapMessage.MessageID);
	
	if ( ldapMessage.MessageType == LDAPMessageType_t::BindResponse )
		messageStructure = struct_to_hex(ldapMessage.LDAPResult, Codes::BindResponse);
		
	else if ( ldapMessage.MessageType == LDAPMessageType_t::SearchResultEntry )
		messageStructure = struct_to_hex(ldapMessage.SearchResultEntry);
		
	else if ( ldapMessage.MessageType == LDAPMessageType_t::SearchResultDone )
		messageStructure = struct_to_hex(ldapMessage.LDAPResult, Codes::SearchResultDone);
	
	else throw runtime_error("Unrecognized message type to create.");
	
	
	ret = Codes::LDAPMessage;
	ret += size_to_hex( messageID.length() + messageStructure.length() );
	ret += messageID;
	ret += messageStructure;
	
	return ret;
}
