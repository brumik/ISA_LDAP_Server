# File about testing
This file contains what should we test and how I done it.

## How and where am I testing?
All tests were run on servers merlin.fit.vutbr.cz and eva.fit.vutbr.cz.
Merlin was the one where the server was started and from eva I used
ldapsearch util to send queries.

Test server is running on port **38900** and with **isa2017-ldap.csv**.

## What to test?
For the next points I test search with mail, uid and name.

* Exact Match
* Substrings
	* Initial, any, final
	* Only initial
	* Only any (one, multiple)
	* Only final
	* Initial and any
	* Initial and final
	* Any and final
* And
* Or
* Not
* Combinations of logical operators
	* And, And
	* And, Or
	* And, Not
	* Or, Or
	* Or, Not
	* And, Or, Not - one for NOT in AND and another for NOT in OR.
* No entry returned
* No filter supported

**For testing I use names not containing non ASCII characters for
easier evaluation (since names with UTF-8 chars are displayed weirdly in
ldapsearch).**

## The test and the expected results
search is alias for:
ldapsearch -h merlin.fit.vutbr.cz -p 38900 -s sub -b base -x

1. **Or, And + Exact Match + Substring: only final**
	* search '(|(uid=xberky02)(&(mail=xklem*)(cn=*Jakub)))'
	* excepted result: xberky02, xkleme11
2. **And, Or, Not + Substring: (Init, any, final / Only Init / Init, any)**
	* search '(& (| (cn=Ud\*dy\*M\*io)(uid=xtich\*)(mail=xtamas01\*fit\*) )(!(uid=xticha04)) )'
	* excepted result: xudvar01, xtamas01 - xticha04 is in NOT
3. **And, And + Or, Or + Substring: Init, final / any**
	* search '(& (& (uid=\*ilva\*)(mail=\*ilva\*@\*))(| (| (uid=xzilva*)(uid=xberky*) )(cn=Zilvar Mi*) ) )'
	* expected result: xzilva02
4. **No entry**
	* search '(uid=xNotF00)'
	* expected result: Only search done response
5. **No filter supported**
	* search
	* expected result: Operations Error