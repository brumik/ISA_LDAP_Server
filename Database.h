/**
 * @file Database.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This static class manages the database.
 * Reading in, filtering and printing.
 */

#ifndef ISA_LDAP_SERVER_FILESEARCH_H
#define ISA_LDAP_SERVER_FILESEARCH_H

#include <string>
#include <vector>

typedef struct database_entry
{
	std::string cn;
	std::string uid;
	std::string mail;
} database_entry;


typedef std::vector<database_entry> database_t;

class Database {
public:
	
	/**
	 * Reads the database from the csv file into structure and returns it.
	 * The csv format should be 'cn;uid;mail \n'
	 *
	 * @param filename Path to the csv file.
	 * @return The database read rom the csv file.
	 */
	static database_t get_db_from_csv(std::string filename);
	
	/**
	 * Filters the given db by Cn.
	 *
	 * @param filter The cn passes the filter if contains this string.
	 * @param db The database to filter.
	 * @return A new database only with entries which passed the filter.
	 */
	static database_t filter_by_cn(const std::string &filter, database_t db);
	
	/**
	 * Creates printable fromat from the database.
	 *
	 * @param db The database to stringify.
	 * @return String with database formatted to 'cn, uid, mail \n'.
	 */
	static std::string toString(database_t db);
};

#endif //ISA_LDAP_SERVER_FILESEARCH_H
