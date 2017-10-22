/**
 * @file MessageID.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This cass contains message ID codes for LDAP
 */

#ifndef ISA_LDAP_SERVER_MESSAGEID_H
#define ISA_LDAP_SERVER_MESSAGEID_H

enum class MessageID {
	BindRequest = 0,
	BindResponse = 1,
	Unbind = 0,
	SearchRequest = 3,
	SearchResEntry = 4,
	SearchResDone = 5
};

#endif //ISA_LDAP_SERVER_MESSAGEID_H
