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
}

std::string Decode::get_next_hex_string(unsigned long num)
{
	if ( (position + num * 2) > hex_message.length() )
		throw runtime_error("Too much of requested number of hex pair.");
	
	string ret = hex_message.substr(position, num * 2);
	position += num*2;
	return ret;
}

unsigned long Decode::get_size()
{
	return stoul(get_next_hex_string(), nullptr, 16);
}

bool Decode::size_control()
{
	return get_size() == (hex_message.length() - position) / 2;
}

unsigned long Decode::get_integer(int max_length)
{
	unsigned long size;
	
	if ( get_next_hex_string() != C_INTEGER )
		throw runtime_error("Integer is not defined.");
	if ( (size = get_size()) > max_length )
		throw length_error("Integer is too long");
	
	return stoul(get_next_hex_string(size), nullptr, 16);
}

std::string Decode::get_string()
{
	string ret;
	unsigned long size;
	
	if ( get_next_hex_string() != C_OCTET_STRING )
		throw runtime_error("Not string.");
	
	size = get_size();
	string hex = get_next_hex_string(size);
	
	for(unsigned long i = 0; i < size * 2 ; i += 2)
		ret.push_back( (char)strtol(hex.substr(i, 2).c_str(), nullptr, 16) );

	return ret;
}

unsigned long Decode::get_messageID()
{
	try {
		return get_integer(4);
	} catch (runtime_error &e) {
		throw runtime_error("MessageID is missing.");
	} catch (length_error &e) {
		throw runtime_error("MessageID is too long.");
	}
}

BindRequest Decode::get_bindRequest()
{
	BindRequest request;
	
	// BindRequest size
	if ( !size_control() )
		throw runtime_error("BindRequest length has different size than specified.");
	
	// Version of client
	try {
		request.Version = get_integer(1);
	} catch (runtime_error &e) {
		throw runtime_error("BindRequest Version is not specified.");
	} catch (length_error &e) {
		throw runtime_error("Version number has too long size.");
	}
	
	// Name of non anonymous client.
	try {
		request.Name = get_string();
	} catch (runtime_error &e) {
		throw runtime_error(string("Getting Bind Name encountered an error: ") + e.what());
	}
	
	// Auth type of client.
	if ( get_next_hex_string(2) != C_SIMPLE_AUTH )
		throw runtime_error("Not simple auth whitout password.");
	
	return request;
}

LDAPMessage Decode::decode_to_struct(std::string &to_decode)
{
	hex_message = to_decode;
	LDAPMessage message;
	
	
	if ( get_next_hex_string() != C_LDAPMessage )
		throw runtime_error("Message is non LDAPMessage.");
	if ( !size_control() )
		throw runtime_error("LDAPMessage has different size than specified.");
	
	message.MessageID = get_messageID();
	
	// Determine what kind of message we got and do the appropriate action.
	string type = get_next_hex_string();
	
	if ( type == C_BindRequest )
		message.BindRequest = get_bindRequest();
	else
		throw runtime_error("Unrecognized message type.");
	
	
	return message;
}
