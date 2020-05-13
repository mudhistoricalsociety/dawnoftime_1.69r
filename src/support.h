/**************************************************************************/
// support.h - 
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#ifndef SUPPORT_H
#define SUPPORT_H

typedef struct letgain_data
{
    struct	letgain_data * next;
    char *	name;           // name of player 
    long    id;             // characters id 
	char *	current_short;  // current short description
	char *	requested_short;          
    char *  requested_long;
	time_t	requested_date;
	sh_int	requested_level;
	sh_int	requested_alliance;
	sh_int	requested_tendency;
    int		flags;
	char *	answered_by;
	time_t	granted_date;
	sh_int	granted_moot;
	char *  granted_short;
	char *  granted_long;
	time_t	letgain_date;
	char *  denied_reason;
	char *  history;
} LETGAIN_DATA;


// prototypes
void load_letgain_db(void);

#define LETGAIN_REQUESTED	(A)
#define LETGAIN_GRANTED		(B)
#define LETGAIN_DECLINED	(C)
#define LETGAIN_PENDING     (D)

#endif //SUPPORT_H

