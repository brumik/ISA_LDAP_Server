/**
 * @file Database.cpp
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 */

#include "Database.h"
#include <fstream>
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

string Database::get_entry_variable(const DatabaseEntry& entry, Columns col)
{
	switch ( col ) {
		case CN: return entry.cn;
		case UID: return entry.uid;
		case MAIL: return entry.mail;
		default:
			throw runtime_error("Not existing column in database.");
	}
}

size_t Database::match_string_start(const string &str, const string &match)
{
	if ( str.find(match) == 0 )
		return match.length();
	
	return string::npos;
}

size_t Database::match_string_any(const string &str, const string &match, size_t pos)
{
	return str.find(match, pos);
}

bool Database::match_string_end(const string &str, const string &match, size_t pos)
{
	if ( str.length() - pos >= match.length() )
		return (0 == str.compare(str.length() - match.length(), match.length(), match));
	else
		return false;
}

Database Database::filter_by_substrings(const string &init, const vector<string> &any_v, const string &end, Columns col)
{
	Database passed_entries;
	size_t curr_pos;
	string entry_value;
	bool passed;
	
	
	for (auto &entry : db) {
		curr_pos = 0;
		entry_value = get_entry_variable(entry, col);
		
		// Check init
		if ( !init.empty() ) {
			curr_pos = match_string_start(entry_value, init);
			
			// If not found did not passed the filter
			if ( curr_pos == string::npos ) continue;
		}
		
		// Check any
		if ( !any_v.empty() ) {
			passed = true;
			
			for (auto &any : any_v)
				if ( !any.empty() ) {
					curr_pos = match_string_any(entry_value, any, curr_pos);
					
					// If any of them fails then not passed and don't have to continue.
					if ( curr_pos == string::npos ) {
						passed = false;
						break;
					}
				}
			
			// If did not passed continue.
			if ( !passed ) continue;
		}
		
		if ( !end.empty() )
			if ( !match_string_end(entry_value, end, curr_pos) )
				continue;
		
		// If gets here all filters are passed and we can add it.
		passed_entries.add_entry(entry);
	}
	
	return passed_entries;
}

Database Database::filter_by_exact(Columns col, const string &filter)
{
	Database passed_entries;
	
	for (auto &entry : this->db)
		if ( get_entry_variable(entry, col) == filter )
			passed_entries.add_entry(entry);
	
	return passed_entries;
}

Database Database::filter_by_exact_cn(const string &filter)
{
	return this->filter_by_exact(Columns::CN, filter);
}

Database Database::filter_by_exact_uid(const string &filter)
{
	return this->filter_by_exact(Columns::UID, filter);
}

Database Database::filter_by_exact_mail(const string &filter)
{
	return this->filter_by_exact(Columns::MAIL, filter);
}

Database Database::filter_by_cn_substrings(const string &init, const vector<string> &any_v, const string &end)
{
	return filter_by_substrings(init, any_v, end, Columns::CN);
}

Database Database::filter_by_uid_substrings(const string &init, const vector<string> &any_v, const string &end)
{
	return filter_by_substrings(init, any_v, end, Columns::UID);
}

Database Database::filter_by_mail_substrings(const string &init, const vector<string> &any_v, const string &end)
{
	return filter_by_substrings(init, any_v, end, Columns::MAIL);
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