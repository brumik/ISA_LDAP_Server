/**
 * @file Error.cpp
 * @author Levente Berky
 *
 * File containing the definitions of error class.
 */

#include "Error.h"
#include "Codes.h"

Error::Error()
{
	code = ResultCode_e::SUCCESS;
	message = "";
}

Error::Error(unsigned int code, const std::string &message)
{
	this->code = static_cast<ResultCode_e>(code);
	this->message = message;
}

void Error::set_error(unsigned code, const std::string &message)
{
	this->code = static_cast<ResultCode_e>(code);
	this->message = message;
}

ResultCode_e Error::get_code()
{
	return code;
}

std::string Error::get_message()
{
	return message;
}

void Error::clear()
{
	code = ResultCode_e::SUCCESS;
	message = "";
}
