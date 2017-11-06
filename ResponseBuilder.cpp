//
// Created by levente on 11/6/17.
//

#include <iostream>
#include "ResponseBuilder.h"

using namespace std;

ResponseBuilder::ResponseBuilder(Database &database)
{
	bind = false;
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

Database ResponseBuilder::request_filter(Filter_t filter)
{
	Database entries;
	
	if ( filter.Type == FilterType_e::EqualityMatch ) {
		if ( filter.EqualityMatch.AttributeDesc == Database::Type_CN )
			entries = db.filter_by_cn(filter.EqualityMatch.AssertionValue, true);
		else if ( filter.EqualityMatch.AttributeDesc == Database::Type_UID )
			entries = db.filter_by_uid(filter.EqualityMatch.AssertionValue, true);
		else if ( filter.EqualityMatch.AttributeDesc == Database::Type_MAIL )
			entries = db.filter_by_mail(filter.EqualityMatch.AssertionValue, true);
		else
			throw runtime_error("Requested filter containing non existing attribute.");
	} else if ( filter.Type == FilterType_e::Substrings ) {
		// TODO
	}
	
	
	return entries;
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
	}
	
	return messages;
}

LDAPMessage_t ResponseBuilder::generate_response(const LDAPMessage_t &request, Error error)
{
	LDAPMessage_t message;
	message.MessageID = request.MessageID;
	
	// Here we wont check if ID us already used because we already sending error back.
	
	if ( request.MessageType == LDAPMessageType_t::BindRequest ) {
		matchedDN = request.BindRequest.Name;
		message.MessageType = LDAPMessageType_t::BindResponse;
	} else if ( request.MessageType == LDAPMessageType_t::SearchRequest )
		message.MessageType = LDAPMessageType_t::SearchResultDone;
	
	message.LDAPResult = create_result(error);
	return message;
}
