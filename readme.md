# Effectiveness
The server is heavy on memory. Reads all the database in and then always searches for all entries specified in filters.
The size limit is checked before returning the array with found entries.

# Behavioral notes.
Server ignoring username and password even it is passed. 
Server returning mostly PROTOCOL_ERROR and AUTH_NOT_SUPPORTED.
Server does not checks the order of request. However it is essential to get unbind request to close the serving process.
Search request coming without bind request considered as anonymous.