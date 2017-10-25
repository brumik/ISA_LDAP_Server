/**
 * @file Database.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This static class manages the database.
 * Reading in, filtering and printing.
 *
 * Usage:
 * Create an db from file: Database db(fileaname);
 * Then if you wan filter by cn AND mail just use: db = db.filter_by_cn('xx').filter_by_mail('xy');
 * If you don't want to lose the main db just don't overwrite it. The filters do not modify the class.
 */

#ifndef ISA_LDAP_SERVER_FILESEARCH_H
#define ISA_LDAP_SERVER_FILESEARCH_H

#include <string>
#include <vector>

class Database {
private:
	struct DatabaseEntry {
		std::string cn;
		std::string uid;
		std::string mail;
		
		bool operator<(const DatabaseEntry& rhs) const
		{
			return uid < rhs.uid;
		}
		
		bool operator==(const DatabaseEntry& rhs) const
		{
			return uid == rhs.uid;
		}
	};
	
	std::vector<DatabaseEntry> db;
	
protected:
	
	/**
	 * Adds a single entry to the db.
	 *
	 * @param entry The entry to adding.
	 */
	void add_entry(const DatabaseEntry &entry);
	
	/**
	 * Actually only converts type string to the corresponding entry field.
	 *
	 * @param entry The entry which field is returned.
	 * @param type The type of the field returned.
	 * @return All field are strings, so we return the field as string.
	 */
	static std::string get_entry_variable(const DatabaseEntry& entry, const std::string& type);
	
	/**
	 * Filters the db by the given value the given entry type.
	 *
	 * @param type The type of the filtered entry value ... uid, mail, cn.
	 * @param filter The value passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 */
	Database filter_by(const std::string& type, const std::string& filter);
	
	/**
	 * Filters the db by the given filter. Matches only exact strings.
	 *
	 * @param type The type of the filtered entry value ... uid, mail, cn.
	 * @param filter The value passes the filter if matches this string.
	 * @return A new database only with entries which passed the filter.
	 */
	Database filter_by_exact(const std::string& type, const std::string& filter);
	
public:
	Database()=default;
	
	/**
	 * Reads the database from the csv file into structure and returns it.
	 * The csv format should be 'cn;uid;mail \n'
	 *
	 * @param filename Path to the csv file.
	 * @return The database read rom the csv file.
	 */
	explicit Database(const std::string& filename);
	
	/**
	 * Filters the given db by Cn.
	 *
	 * @param filter The cn passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_cn(const std::string& filter, bool exact=false);
	
	/**
	 * Filters the given db by uid.
	 *
	 * @param filter The uid passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_uid(const std::string& filter);
	
	/**
	 * Filters the given db by Mail.
	 *
	 * @param filter The mail passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_mail(const std::string& filter);
	
	/**
	 * Filters database to not to contain given database.
	 * Uses vector difference.
	 *
	 * @param exclude The database not con contain the result database.
	 * @return Database without excluded db.
	 */
	Database filter_not(Database exclude);
	
	/**
	 * Intersects two databases without duplication simulating AND operation.
	 * Uses vector intersection.
	 *
	 * @param intersect The database to merge with.
	 * @return Database merged with the given one.
	 */
	Database filter_and(Database intersect);
	
	/**
	 * Unions two databases without duplication simulating OR operation.
	 * Uses vector union.
	 *
	 * @param db_union Database to union with.
	 * @return New database containing all entries from both databases not duplicated.
	 */
	Database filter_or(Database db_union);
	
	/**
	 * Creates printable format from the database.
	 *
	 * @return String with database formatted to 'cn, uid, mail \n'.
	 */
	std::string toString();
};

#endif //ISA_LDAP_SERVER_FILESEARCH_H
