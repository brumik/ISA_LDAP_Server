/**
 * @file Database.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This class manages the database.
 * Reading in, filtering and printing.
 *
 * Usage:
 * Create an db from file: Database db(fileaname);
 * Then if you wan filter by cn AND mail just use: db = db.filter_by_cn('xx').filter_by_mail('xy');
 * If you don't want to lose the main db just don't overwrite it. The filters do not modify the class.
 */

#ifndef ISA_LDAP_SERVER_DATABASE_H
#define ISA_LDAP_SERVER_DATABASE_H

#include <string>
#include <vector>

using namespace std;

class Database {
private:
	enum Columns { CN, UID, MAIL };
	struct DatabaseEntry {
		string cn;
		string uid;
		string mail;
		
		bool operator<(const DatabaseEntry& rhs) const
		{
			return uid < rhs.uid;
		}
		
		bool operator==(const DatabaseEntry& rhs) const
		{
			return uid == rhs.uid;
		}
	};
	
	vector<DatabaseEntry> db;
	
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
	 * @param col The type of the field returned.
	 * @return All field are strings, so we return the field as string.
	 */
	static string get_entry_variable(const DatabaseEntry& entry, Columns col);
	
	/**
	 * Filters the db by the given filter. Matches only exact strings.
	 *
	 * @param col The type of the filtered entry value ... uid, mail, cn.
	 * @param filter The value passes the filter if matches this string.
	 * @return A new database only with entries which passed the filter.
	 */
	Database filter_by_exact(Columns col, const string& filter);
	
	/**
	 * Searches the given string with match.
	 *
	 * @param str The string to search for start.
	 * @param match The string which we are looking for.
	 * @return The position where the string ends or -1 if not found.
	 */
	size_t match_string_start(const string &str, const string &match);
	
	/**
	 * Searches the given string with match anywhere.
	 *
	 * @param str The string to search for start.
	 * @param match The string which we are looking for.
	 * @param pos The positing where to start te search.
	 * @return The position where the string ends or -1 if not found.
	 */
	size_t match_string_any(const string &str, const string &match, size_t pos);
	
	/**
	 * Searches the given string end with match.
	 *
	 * @param str The string to search for ending.
	 * @param match The string which we are looking for.
	 * @param pos The positing where to start te search.
	 * @return True if there is a match. Otherwise false.
	 */
	bool match_string_end(const string &str, const string &match, size_t pos);
	
	/**
	 * Matches a substring query. Matches 'This is a text' as 'Thi*s*a*xt' where
	 * init is 'Thi', any are 's' and 'a' and end is 'xt'.
	 * 
	 * Inside this function we test against end of the string.
	 * If any of filters: start, (any of any_v), end reaches end of the string we 
	 * know that did not passed the filter. Otherwise we add to the new database.
	 *
	 * I did not broke this function into smaller ones because its pretty obvious in this form too.
	 * 
	 * @param init The entire has to start with this.
	 * @param any The entry most contain these strings in the order they are.
	 * @param end The entry has to end with this string.
	 * @param col The column to match the filter.
	 * @return  A new database only whit entries passed by this filter.
	 */
	Database filter_by_substrings(const string &init, const vector<string> &any_v,
	                              const string &end, Columns col);
	
public:
	static constexpr const char* Type_CN = "cn";
	static constexpr const char* Type_UID = "uid";
	static constexpr const char* Type_MAIL = "mail";
	
	/**
	 * Constructor.
	 */
	Database()=default;
	
	/**
	 * Reads the database from the csv file into structure and returns it.
	 * The csv format should be 'cn;uid;mail \n'
	 *
	 * @param filename Path to the csv file.
	 * @return The database read rom the csv file.
	 */
	explicit Database(const string& filename);
	
	/**
	 * Filters the given db by Cn.
	 *
	 * @param filter The cn passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_exact_cn(const string &filter);
	
	/**
	 * Filters the given db by uid.
	 *
	 * @param filter The uid passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_exact_uid(const string &filter);
	
	/**
	 * Filters the given db by Mail.
	 *
	 * @param filter The mail passes the filter if contains this string.
	 * @return A new database only with entries which passed the filter.
	 * @sa filter_by()
	 */
	Database filter_by_exact_mail(const string &filter);
	
	/**
	 * Calls the filter by function on specified column.
	 * 
	 * @param init The entire has to start with this.
	 * @param any The entry most contain these strings in the order they are.
	 * @param end The entry has to end with this string.
	 * @return  A new database only whit entries passed by this filter.
	 */
	Database filter_by_cn_substrings(const string &init, const vector<string> &any_v, const string &end);
	
	/**
	 * Calls the filter by function on specified column.
	 * 
	 * @param init The entire has to start with this.
	 * @param any The entry most contain these strings in the order they are.
	 * @param end The entry has to end with this string.
	 * @return  A new database only whit entries passed by this filter.
	 */
	Database filter_by_uid_substrings(const string &init, const vector<string> &any_v, const string &end);
	
	/**
	 * Calls the filter by function on specified column.
	 * 
	 * @param init The entire has to start with this.
	 * @param any The entry most contain these strings in the order they are.
	 * @param end The entry has to end with this string.
	 * @return  A new database only whit entries passed by this filter.
	 */
	Database filter_by_mail_substrings(const string &init, const vector<string> &any_v, const string &end);
	
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
	 * Returns the entries as iterable.
	 * @return The db.
	 */
	vector<DatabaseEntry> get_entries();
	
	/**
	 * Creates printable format from the database.
	 *
	 * @return String with database formatted to 'cn, uid, mail \n'.
	 */
	string toString();
};

#endif //ISA_LDAP_SERVER_DATABASE_H
