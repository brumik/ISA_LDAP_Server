//
// Created by levente on 11/6/17.
//

#include <iostream>
#include "ResponseBuilder.h"

using namespace std;

ResponseBuilder::ResponseBuilder(Database &database)
{
	matchedDN = "";
	db = database;
}

LDAPResult_t ResponseBuilder::create_result(Error &error)
{
	LDAPResult_t result;
	result.ResultCode = error.get_code();
	result.ErrorMessage = error.get_message();
	result.MatchedDN = matchedDN;
	return result;
}

Database ResponseBuilder::request_filter_substring(const Substrings_t &substrings)
{
	string init, end;
	vector<string> any;
	
	// Build search strings and vectors.
	for ( auto &item : substrings.items )
		if ( item.Type == Substring_Item_Type_e::Initial )
			init = item.data;
		else if ( item.Type == Substring_Item_Type_e::Any )
			any.push_back(item.data);
		else if ( item.Type == Substring_Item_Type_e::Final )
			end = item.data;
		else throw runtime_error("ResponseBuilder: Invalid Substring Item Type.");
	
	if ( substrings.Type == Database::Type_CN )
		return db.filter_by_cn_substrings(init, any, end);
	else if ( substrings.Type == Database::Type_UID )
		return db.filter_by_uid_substrings(init, any, end);
	else if ( substrings.Type == Database::Type_MAIL )
		return db.filter_by_mail_substrings(init, any, end);
	else throw runtime_error("Response Builder: EqualityMatch containing non existing attribute.");
}

Database ResponseBuilder::request_filter_equality(const AttributeValueAssertion_t &equality)
{
	if ( equality.AttributeDesc == Database::Type_CN )
		return db.filter_by_exact_cn(equality.AssertionValue);
	
	else if ( equality.AttributeDesc == Database::Type_UID )
		return db.filter_by_exact_uid(equality.AssertionValue);
	
	else if ( equality.AttributeDesc == Database::Type_MAIL )
		return db.filter_by_exact_mail(equality.AssertionValue);
	
	else throw runtime_error("Response Builder: EqualityMatch containing non existing attribute.");
}

Database ResponseBuilder::request_filter(const Filter_t &filter)
{
	if ( filter.Type == FilterType_e::EqualityMatch ) {
		return request_filter_equality(filter.EqualityMatch);
		
	} else if ( filter.Type == FilterType_e::Substrings ) {
		return request_filter_substring(filter.Substrings);
		
	} else if ( filter.Type == FilterType_e::And ) {
		Database temp = db;
		for ( const Filter_t &f : filter.And)
			temp = temp.filter_and( request_filter(f) );
		
		return temp;
		
	} else if ( filter.Type == FilterType_e::Or ) {
		Database temp;
		for ( const Filter_t &f : filter.Or)
			temp = temp.filter_or( request_filter(f) );
		
		return temp;
		
	} else if ( filter.Type == FilterType_e::Not ) {
		return db.filter_not( request_filter( filter.Not.at(0) ) );
	
	} else throw runtime_error("Filter type not supported.");
}

vector<LDAPMessage_t> ResponseBuilder::generate_entries_response(const LDAPMessage_t &request)
{
	vector<LDAPMessage_t> messages;
	
	Database entries = request_filter(request.SearchRequest.Filter);
	
	for ( auto entry : entries.get_entries() ) {
		// Create message for the entry
		LDAPMessage_t message;
		message.MessageID = request.MessageID;
		message.MessageType = LDAPMessageType_t::SearchResultEntry;

		// Set ObjectName
		PartialAttributeList_t attr;
		message.SearchResultEntry.ObjectName = entry.uid;

		// Fill with CN
		attr.Type = Database::Type_CN;
		attr.Values.emplace_back(entry.cn);
		message.SearchResultEntry.Attributes.push_back(attr);

		attr.Values.clear();

		// Fill with MAIL
		attr.Type = Database::Type_MAIL;
		attr.Values.emplace_back(entry.mail);
		message.SearchResultEntry.Attributes.push_back(attr);

		// Save message
		messages.push_back(message);
		if ( request.SearchRequest.SizeLimit != 0)
			if ( messages.size() >= request.SearchRequest.SizeLimit )
				break;
	}
	
	return messages;
}

LDAPMessage_t ResponseBuilder::generate_response(const LDAPMessage_t &request, Error error)
{
	LDAPMessage_t message;
	message.MessageID = request.MessageID;
	
	// Here we wont check if ID us already used because we already sending error back.
	
	if ( request.MessageType == LDAPMessageType_t::BindRequest ) {
		message.MessageType = LDAPMessageType_t::BindResponse;
	} else if ( request.MessageType == LDAPMessageType_t::SearchRequest ) {
		message.MessageType = LDAPMessageType_t::SearchResultDone;
	}
	
	message.LDAPResult = create_result(error);
	return message;
}
