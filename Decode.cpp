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

std::string Decode::get_next_hex_string(unsigned long num)
{
	if ((position + num * 2) > hex_message.length())
		throw runtime_error("Too much of requested number of hex pair.");
	
	string ret = hex_message.substr(position, num * 2);
	position += num * 2;
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
	
	if ( get_next_hex_string() != Codes::INTEGER )
		throw runtime_error("Integer is not defined.");
	if ((size = get_size()) > max_length )
		throw length_error("Integer is too long");
	
	return stoul(get_next_hex_string(size), nullptr, 16);
}

std::string Decode::get_string()
{
	string ret;
	unsigned long size;
	
	if ( get_next_hex_string() != Codes::OCTET_STRING )
		throw runtime_error("Not string.");
	
	size = get_size();
	string hex = get_next_hex_string(size);
	
	for( unsigned long i = 0; i < size * 2; i += 2 )
		ret.push_back((char) strtol(hex.substr(i, 2).c_str(), nullptr, 16));
	
	return ret;
}

unsigned long Decode::get_messageID()
{
	try {
		return get_integer(4);
	} catch (runtime_error &e) {
		error.set_error(Error::PROTOCOL_ERROR, "MessageID is missing.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(Error::PROTOCOL_ERROR, "MessageID is too long.");
		throw runtime_error(error.get_message());
	}
}

BindRequest Decode::get_bindRequest()
{
	BindRequest request;
	
	// BindRequest size
	if ( !size_control() ) {
		error.set_error(Error::PROTOCOL_ERROR, "BindRequest length has different size than specified.");
		throw runtime_error(error.get_message());
	}
	
	// Version of client
	try {
		request.Version = get_integer(1);
		
		// If version is not 2 or 3 then it is not supported.
		if ( !(request.Version == 3 || request.Version == 2) ) {
			error.set_error(Error::PROTOCOL_ERROR, "Protocol version is not supported.");
			throw runtime_error(error.get_message());
		}
		
	} catch (runtime_error &e) {
		error.set_error(Error::PROTOCOL_ERROR, "BindRequest Version is not specified.");
		throw runtime_error(error.get_message());
	} catch (length_error &e) {
		error.set_error(Error::PROTOCOL_ERROR, "Version number has too long size.");
		throw runtime_error(error.get_message());
	}
	
	// Name of non anonymous client.
	try {
		request.Name = get_string();
	} catch (runtime_error &e) {
		error.set_error(Error::PROTOCOL_ERROR, string("Getting Bind Name encountered an error: ") + e.what());
		throw runtime_error(error.get_message());
	}
	
	// Auth type of client.
	if ( get_next_hex_string(2) != Codes::SIMPLE_AUTH ) {
		error.set_error(Error::AUTH_NOT_SUPPORTED, "Not simple auth whiteout password.");
		throw runtime_error(error.get_message());
	}
	
	return request;
}

LDAPMessage Decode::decode_to_struct(std::string &to_decode)
{
	// Reset error message.
	error.clear();
	hex_message = to_decode;
	LDAPMessage message;
	
	
	if ( get_next_hex_string() != Codes::LDAPMessage ) {
		error.set_error(Error::PROTOCOL_ERROR, "Message is non LDAPMessage.");
		throw runtime_error(error.get_message());
	}
	
	if ( !size_control() ) {
		error.set_error(Error::PROTOCOL_ERROR, "LDAPMessage has different size than specified.");
		throw runtime_error(error.get_message());
	}
	
	message.MessageID = get_messageID();
	
	// Determine what kind of message we got and do the appropriate action.
	string type = get_next_hex_string();
	
	if ( type == Codes::BindRequest ) {
		message.MessageType = Codes::BindRequest;
		message.BindRequest = get_bindRequest();
	} else {
		error.set_error(Error::PROTOCOL_ERROR, "Unrecognized message type.");
		throw runtime_error(error.get_message());
	}
	
	return message;
}

Error Decode::get_error()
{
	return error;
}
