/**************************************************************************/
// pload.h - dawn player loading/unloading system
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef PLOAD_H
#define PLOAD_H

void pload_extract(char_data *ch, char_data *victim);
char_data *pload_find_player_by_name(char *name);

struct pload_data
{
	bool dont_save; // if true, the pfile wont be saved
	char_data *loaded_by; // the imm that loaded the player file	
};

#endif // PLOAD_H

