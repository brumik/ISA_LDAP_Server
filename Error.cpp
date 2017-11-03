/**
 * @file Error.cpp
 * @author Levente Berky
 *
 * File containing the definitions of error class.
 */

#include "Error.h"

Error::Error()
{
	code = SUCCESS;
	message = "";
}

void Error::set_error(unsigned code, const std::string &message)
{
	this->code = code;
	this->message = message;
}

unsigned Error::get_code()
{
	return code;
}

std::string Error::get_message()
{
	return message;
}

void Error::clear()
{
	code = SUCCESS;
	message = "";
}
