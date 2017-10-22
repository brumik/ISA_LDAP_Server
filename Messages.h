/**
 * @file Messages.h
 * @author Levente Berky
 * @email xberky02@stud.fit.vutbr.cz
 *
 * This file contains LDAP structures to send.
 */

#ifndef ISA_LDAP_SERVER_MESSAGES_H
#define ISA_LDAP_SERVER_MESSAGES_H

#include "MessageID.h"

class Messages {
	typedef struct LDAPMessage {
		int messageID;
		/* Some specific things */
	} LDAPMessage;
};


#endif //ISA_LDAP_SERVER_MESSAGES_H
