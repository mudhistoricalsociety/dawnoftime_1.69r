/**************************************************************************/
// resolve.h 
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#ifndef RESOLVE_H
#define RESOLVE_H

void resolver_init( char *mainpath );
void resolver_query( connection_data *desc );
void resolver_apply_results(socklen_t ipaddr, char *results);
void resolver_get_response( void );
void do_rebootresolver(char_data *ch, char *);

class resolve_result_address
{
public:
	resolve_result_address();
	~resolve_result_address();

	void add(char *address, bool ipv6);
	resolve_result_address *get(int count);

	char *address;
	bool ipv6;
	resolve_result_address *next;
};
extern resolve_result_address *resolve_result_address_list;
extern bool resolver_address_found;
extern bool resolver_address_failed;


#endif // RESOLVE_H

