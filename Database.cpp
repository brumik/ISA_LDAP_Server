/**
 * @file Database.cpp
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 */

#include "Database.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

database_t Database::get_db_from_csv(string filename)
{
	database_t db;
	ifstream inStream;
	database_entry temp;
	
	try {
		inStream.open(filename);
		
		if ( !inStream ) throw runtime_error("File not found or could not be opened.");
		
		while   (	getline(inStream, temp.cn, ';') &&
		            getline(inStream, temp.uid,   ';') &&
		            getline(inStream, temp.mail)
				)
		{
			db.push_back(temp);
		}
		
		inStream.close();
	} catch (...) {
		throw;
	}
	
	return db;
}

database_t Database::filter_by_cn(const std::string &filter, database_t db)
{
	database_t passed_entries;
	
	for (auto &entry : db)
	{
		if ( entry.cn.find(filter) != string::npos )
			passed_entries.push_back(entry);
	}
	
	return passed_entries;
}

std::string Database::toString(database_t db)
{
	stringstream ofs;
	
	for (auto &entry : db)
	{
		ofs << entry.cn << ", " << entry.uid << ", " << entry.mail << endl;
	}
	
	return ofs.str();
}