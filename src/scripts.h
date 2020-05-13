/**************************************************************************/
// scripts.h - 
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef SCRIPTS_H
#define SCRIPTS_H

typedef struct script_data SCRIPT_DATA;

struct script_data
{
	script_data *next;
	char *script_name;
	char *auth_users;
};

void load_script_db( void );

#endif // SCRIPTS_H
