/**
 * @file Database.cpp
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 */

#include "Database.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Database::Database(const string& filename)
{
	ifstream inStream;
	DatabaseEntry temp;
	
	try {
		inStream.open(filename);
		
		if ( !inStream ) throw runtime_error("File not found or could not be opened.");
		
		while   (	getline(inStream, temp.cn, ';') &&
		            getline(inStream, temp.uid,   ';') &&
		            getline(inStream, temp.mail)
				)
		{
			this->add_entry(temp);
		}
		
		inStream.close();
	} catch (...) {
		throw;
	}
}

void Database::add_entry(const DatabaseEntry &entry)
{
	db.push_back(entry);
}

string Database::get_entry_variable(const DatabaseEntry& entry, const string& type)
{
	if ( type == "cn" )
		return entry.cn;
	if ( type == "uid" )
		return entry.uid;
	if ( type == "mail" )
		return entry.mail;
	
	throw runtime_error("Not existing column in database.");
}

long Database::match_string_start(std::string str, std::string match)
{
	if (str.find(match) == 0)
		return match.length();
	
	return -1;
}

bool Database::match_string_end(std::string str, std::string match)
{
	if (str.length() >= match.length())
		return (0 == str.compare(str.length() - match.length(), match.length(), match));
	else
		return false;
}

Database Database::filter_by(const std::string& type, const std::string &filter)
{
	Database passed_entries;
	
	for (auto &entry : this->db)
		if ( get_entry_variable(entry, type).find(filter) != string::npos )
			passed_entries.add_entry(entry);
	
	return passed_entries;
}

Database Database::filter_by_exact(const std::string &type, const std::string &filter)
{
	Database passed_entries;
	
	for (auto &entry : this->db)
		if ( get_entry_variable(entry, type) == filter )
			passed_entries.add_entry(entry);
	
	return passed_entries;
}

Database Database::filter_by_cn(const std::string &filter, bool exact)
{
	if ( exact )
		return this->filter_by_exact(Type_CN, filter);
	else
		return this->filter_by(Type_CN, filter);
}

Database Database::filter_by_uid(const std::string &filter, bool exact)
{
	if ( exact )
		return this->filter_by_exact(Type_UID, filter);
	else
		return this->filter_by(Type_UID, filter);
}

Database Database::filter_by_mail(const std::string &filter, bool exact)
{
	if ( exact )
		return this->filter_by_exact(Type_MAIL, filter);
	else
		return this->filter_by(Type_MAIL, filter);
}

Database Database::filter_not(Database exclude)
{
	Database passed_entries;
	
	sort(exclude.db.begin(), exclude.db.end());
	sort(db.begin(), db.end());
	
	set_difference(db.begin(), db.end(),
	               exclude.db.begin(), exclude.db.end(),
	               back_inserter(passed_entries.db));
	
	return passed_entries;
}

Database Database::filter_and(Database intersect)
{
	Database passed_entries;
	
	sort(intersect.db.begin(), intersect.db.end());
	sort(db.begin(), db.end());
	
	set_intersection(intersect.db.begin(), intersect.db.end(),
	                 db.begin(), db.end(),
	                 back_inserter(passed_entries.db));
	
	return passed_entries;
}

Database Database::filter_or(Database db_union)
{
	Database passed_entries;
	
	sort(db_union.db.begin(), db_union.db.end());
	sort(db.begin(), db.end());
	
	set_union(db_union.db.begin(), db_union.db.end(),
	                 db.begin(), db.end(),
	                 back_inserter(passed_entries.db));
	
	return passed_entries;
}

vector<Database::DatabaseEntry> Database::get_entries()
{
	return db;
}

string Database::toString()
{
	stringstream ofs;
	
	for (auto &entry : db)
		ofs << entry.cn << ", " << entry.uid << ", " << entry.mail << endl;
	
	return ofs.str();
}