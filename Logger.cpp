//
// Created by levente on 11/5/17.
//

#include <sstream>
#include "Logger.h"

using namespace std;

string Logger::print_substrings(const Substrings_t &substrings)
{
	stringstream stream;
	stream << "\t\tType: " << substrings.Type << endl;
	stream << "\t\tItems: {" << endl;
	for ( auto item : substrings.items )
		stream << "\t\t\tType: " << item.Type << endl << "\t\t\tData: " << item.data << endl;
	stream << "\t\t}" << endl;
	
	return stream.str();
}

string Logger::print_filter(const Filter_t &filter)
{
	stringstream stream;
	stream << "\tType: " << filter.Type << endl;
	if ( filter.Type == FilterType_e::And )
		stream << "\tAnd: TODO" << endl;
	else if ( filter.Type == FilterType_e::Or )
		stream << "\tOr: TODO" << endl;
	else if ( filter.Type == FilterType_e::Not )
		stream << "\tNot: TODO" << endl;
	else if ( filter.Type == FilterType_e::EqualityMatch )
		stream << "\tEqualityMatch: " << filter.EqualityMatch.AttributeDesc << " -> " << filter.EqualityMatch.AssertionValue << endl;
	else if ( filter.Type == FilterType_e::Substrings ) {
		stream << "\tSubstrings {" << endl;
		stream << print_substrings(filter.Substrings);
		stream << "\t}" << endl;
	} else
		stream << "Unrecognized Type" << endl;
	
	
	return stream.str();
}

string Logger::print_search_request(const SearchRequest_t &request)
{
	
	stringstream stream;
	stream << "BaseObject: " << request.BaseObject << endl;
	stream << "Scope: " << request.Scope << endl;
	stream << "DerefAlias: " << request.DerefAliases << endl;
	stream << "Size/Time limit: " << request.SizeLimit << "/" << request.TimeLimit << endl;
	stream << "TypesOnly: " << request.TypesOnly << endl;
	stream << "Filter: {" << endl;
	stream << print_filter(request.Filter);
	stream << "}" << endl;
	
	return stream.str();
}

string Logger::print_ldap_message(const LDAPMessage_t &ldapMessage)
{
	stringstream stream;
	if ( ldapMessage.MessageType == LDAPMessageType_t::SearchRequest ) {
		stream << "SearchRequest ID: " << ldapMessage.MessageID << endl;
		stream << print_search_request(ldapMessage.SearchRequest);
	}
	
	return stream.str();
}