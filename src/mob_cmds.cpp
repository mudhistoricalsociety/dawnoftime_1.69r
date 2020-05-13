/**************************************************************************/
// mob_cmds.cpp - Mobprogs commands, code greatly enhanced by Kal
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * >> Original Diku Mud copyright (c)1990, 1991 by Sebastian Hammer,       *
 *    Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, & Katja Nyboe.   *
 * >> Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael       *
 *    Chastain, Michael Quan, and Mitchell Tse.                            *
 * >> ROM 2.4 is copyright 1993-1995 Russ Taylor and has been brought to   *
 *    you by the ROM consortium: Russ Taylor(rtaylor@pacinfo.com),         *
 *    Gabrielle Taylor(gtaylor@pacinfo.com) & Brian Moore(rom@rom.efn.org) *
 * >> Oblivion 1.2 is copyright 1996 Wes Wagner                            *
 **************************************************************************/
/***************************************************************************
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 ***************************************************************************/
#include "include.h"
#include "mob_cmds.h"
#include "security.h"
#include "msp.h"

DECLARE_DO_FUN( do_look 	);
extern ROOM_INDEX_DATA *find_location( char_data *, char * );
DECLARE_DO_FUN( do_silently);
DECLARE_DO_FUN( do_restore);
DECLARE_DO_FUN( do_peace );
DECLARE_DO_FUN( do_remort );

/**************************************************************************/
// Command table.
const	struct	mob_cmd_type	mob_cmd_table	[] =
{
	//{  name,			do_fun,			descript,	
	//										syntax,  
	//										notes,
	//										example},
	{"affect",	do_mpaffect,"Sets tired, hunger and thirst values.",
		"mob affect $n <tired|hunger|thirst> <value>",
		"mob affect $n tired 20 -- check 'help affect_values' to see what the values mean."},
	{"asound",do_mpasound,"Echos the string/sound to all the rooms around the mobile.",
		"mob asound <string>", 
		"The room the mob is in does not see the echo",
		"examples can go in here like this"},
	{"assist",do_mpassist,"Lets the mobile assist another mob or player", 
		"mob assist <character>", 
		"Mob must be able to see other mob/player to be of effect"},
	{"at",do_mpat,"Lets the mobile do a command at another location",
		"mob at <location> <commands>", 
		"Like imm at command"},
	{"call",do_mpcall,"Lets the mobile to call another MOBprogram within a MOBprogram.", 
		"mob call <vnum> [victim|'null'] [object1|'null'] [object2|'null']",
		"Characters and objects past as parameters must be in the same room with the mobile."},
	{"cancel",	do_mpcancel, "Cancels the delay timer set using the \"mob delay\" command.",
		"mob cancel",
		"After \"mob cancel\", the delay trigger program will not be run till another \"mob delay\" command. "},
	{"cast",	do_mpcast,
		"Lets the mobile cast spells",
		"mob cast '<spell name>' [target|normal spell parameters]",
		"\"mob cast\" has less checking than the normal spell cast command, no mana is required and has no lag at all"},
	{"damage",	do_mpdamage,
		"Does a range of damage to a player after taking player armour into account",
		"mob damage <target/all> <lower limit> <upper limit> [lethal]",
		"Lethal parameter is optional, if not used, player will never die from damage.",},
	{"delay",	do_mpdelay,
		"Causes a delay (in ticks) before calling another mprog which has the trigger as delay",
		"mob delay <ticks>",
		"other mprog must be on same mob"},

	{"dequeueall", do_mpdequeueall, "Dequeues all currently running mobprogs.",
		"mob dequeueall"
	},

	{"duplicate",do_dupeobj,"Will duplicate an item.",
		"mob duplicate $o",
		"Used in conjunction with the GIVE trigger, this command can make specialized shopkeepers.",
		"IE, a keycutter, a shoppie that will duplicate keys given to him."},
	{	"dzecho",		do_mpdzecho,		"DEBUG ZONE ECHO - Prints the string to all immortals, and all characters in building mode in the same area as a mob.", 
		"mob dzecho <string>",
		"This is very useful for debugging mobprogs, a disabled mobprog will automatically do a dzecho in place of running a disabled prog."},
	{"echo",	do_mpecho, "Displays string to everyone in the room",
		"mob echo <string>",
		"Good for complimenting mob transfers",
		"mob echo $N is quietly ushered into the room"},
	{"echoaround",do_mpechoaround,"Displays string to everyone else in the room as the target",
		"mob echoaround <target> <string>",
		"The target itself will not see the message",
		"mob echoaround $n $N looks like a complete idiot"},
	{"echoat",do_mpechoat, "Displays string to the target only",
		"mob echoat <target> <string>",
		"used in conjuction with echoaround to give different messages "
		"to the target and his surrounding people","mob echoat $n You feel like an idiot"},
	{"flee",do_mpflee, "causes the mob to unconditionally flee",
		"mob flee",
		"can be used with hit points percentage trigger to simulate \"wimpy\" behavior"},
	{"force",do_mpforce, "forces the target to do the designated command",
		"mob force <target|'all'> <command>",
		"target will not be told they are being forced so add some echo message"},
	{"forget",do_mpforget, "Cancels the target remembered by the \"mob remember\" command",
		"mob forget"},
	{"gecho",do_mpgecho, "Global Echo - Prints the string to all active players in the game", 
		"mob gecho <string>",
		"Same as immortal gecho command, should be used rarely!"},
	{"gforce",do_mpgforce, "Same as \"mob force\" but affects the whole group that target is in",
		"mob gforce <target> <command>",
		"Use with care as command must fit logic or reality"},
	{"goto",do_mpgoto, "Moves the mob to a another room or mob location",
		"mob goto <vnum/mob-name>",
		"There will be no auto message shown so use echos to compliment it,."
		"also if using mob-name, make sure the name is unique",
		"mob goto 3000 - Will move the mob to room 3000."},
	{"gtransfer",do_mpgtransfer, "Same as \"mob transfer\" but moves the whole group the target is in",
		"mob gtransfer <target>",
		"Convenient to use as it will not break up groups like transfer does, also no message will be shown"},
	{"inflict",do_mpinflict, "Deals a range of absolute damage to a target and ignores target armour",
		"mob inflict <target> <lower> <upper> [kill]",
		"Optional kill parameter is to make it possible for damage to kill targets, "
		"ignores level as well as armour","mob inflict xelton 0 100 kill"},
	{"junk",do_mpjunk, "Destroy all of an object in mobs inventory",
		"mob junk <item>",
		"It can also destroy a worn object and it can destroy "
		"items using all.xxxxx or even straight 'mob junk all' to get rid of everything"
		"matching is performed using the item(s) name."
	},

	{"inroom", do_mpinroom,	"Allows a mob to only 'see' players and objects in the same room.",
		"mob inroom <command>",
		"Very useful if you only want to vforce mobs in the existing room, etc."},

	{"letpass",do_mpletpass,"Allows someone pass even if there is an exit or exall trigger.",
		"mob letpass",
		"An alternative to using \"mob transfer\", the movement of characters will echo to "
		"the room, actual move isn't done till the current mobprog is completed, has only an affect in an exit or exall trigger."},

	{"loginprocessed",do_mploginprocessed, "Used with a loginroom or loginarea triggers",
		"mob loginprocessed",
		"Use it to tell the mobprog system to not run any other progs for a login."
	},

	{"logoutprocessed",do_mplogoutprocessed, "Used with a logoutroom or logoutarea triggers",
		"mob loginprocessed",
		"Use it to tell the mobprog system to not run any other progs for a logout."
	},


	{"preventattack",do_mppreventattack,"If used in a prekill trigger, will stop a person initiating combat with the mob.",
		"mob preventattack",
		"Best method to stop a player attacking a mob, the mobprog code that calls this must also do the echos to the player to inform them why they didn't attack."
	},

	{"preventmove",do_mppreventmove,"Stops a mob moving into a room when used in a premove trigger.",
		"mob preventmove",
		"put a premove trigger on a mob (same syntax as an exit trigger, only runs when the mob moves)."
		"The vnum of the room the mob is moving to is in $V (check with 'if value $V == 3001' etc), if you dont want the mob to move there"
		"put 'mob preventmove' in the mobprog at the appropriate place."},


	{"preventprac",do_mppreventprac,"If used in a preprac trigger, will terminate the hard coded practising behaviour.",
		"mob preventprac",
		"Using preventprac to prevent a player from pracing, will not tell them why, you must manually echo that to the player."
	},

	{"preventtrain",do_mppreventtrain,"If used in a pretrain trigger, will terminate the hard coded train behaviour.",
		"mob preventtrain",
		"Using preventtrain to prevent a player from training, will not tell them why, you must manually echo that to the player."
	},


	{"seeall",do_mpseeall,"Put this at the top of a mobprog, and the mob will be able to see everyone (and all objects) in the room",
		"mob seeall",
		"If you want to do a 'mob echoat $n' that MUST get to $n even when the mob can't naturally see $n,"
		" then put 'mob seeall' at the top of your mobprog.  "
		"The affect of this command is automatically canceled at the end of the mobprog, "
		"and does not effect other functions called with mob call."
		"It can not be used to see wizi imms."},
	{"noseeall",do_mpnoseeall,"Cancels the 'mob seeall' command before the function ends.",
		"mob noseeall",
		""},
	{"kill",do_mpkill,"Causes the mob to attack the targeted player",
		"mob kill <target>","Mob must be able to see the target normally"},

	{"mload",do_mpmload, "causes mob to load the specified mob into the room it is in",
			"mob mload <vnum>",
			"no message is automatically shown"},
	{"mpfollow",do_mpfollow, "Allows you to follow a player even when they have nofollow on",
				"  mob mpfollow <victim>",
				"There are no echos from using this command."},
	{"mpsneaky",do_mpsneaky,"Allows any mob to sneak without having to be a thief mob",
				"mob mpsneak [on|off|debug]",
				"The debug option was left over from testing",
				"  mob mpsneak on"},
	{"msptochar",do_mpmsptochar, "Used to send a msp sound to a player",
				"The sound files must be in the http://<mud_url>/mobprog/ directory, or a directory off it.",
				"  mob msptochar $n ikissu.wav`1"
				"              mob msptochar $Q zoo/cow.wav"},
	{"msptoroom",do_mpmsptoroom, "Used to send a msp sound to everyone in the room",
				"The sound files must be in the http://<mud_url>/msp/mobprog/ directory, or a directory off it.",
				"The sounds from an msptoroom are also heard in the surrounding rooms at a reduced volume.\r\n",
				"  mob msptoroom ikissu.wav`1"
				"              mob msptoroom zoo/cow.wav"},
	{"oload",do_mpoload, "causes mob to load the specified object into its inventory",
			"mob oload <vnum> [level] [where_option]",
			"Oload will load an item of the vnum specified.  "
			"The level of the object can optionally be specified."
			"The object will default to loading into the mobs inventory (if no where_option is specified)... "
			"if the where_option is a 'W' (or 'w') then the mob will automatically wear the object... "
			"if the where_option is a 'R' (or 'r') then the object will be loaded into the room."},

	{"otransfer",do_mpotransfer, "transfer object specified in room to another location",
			"mob otransfer <object vnum> <location>",
			"Searchs for the object in the following order:`1"
			"              room of mob, carried by mob, worn by mob.`1"
			"              If worn it will be unequiped first."},
	{"peace",do_peace,"Stops all fights involving characters in the same room",
		"mob peace",
		"Will also remove aggresive bits from the mobs in the same room.",
		"  mob say Fighting you is a waste of my time and energy,`1"
		"                mob say come back when you can hurt me.`1"
		"                mob peace"},
	{"purge",do_mppurge, "destroys an item in the room",
		"mob purge [item/mob]",
		"without stating an argumet (ie. item or mob) all NPCs and items in the room will be purged",
		"  mob emote sends forth a wall of searing flames, clensing the hall of everything!`1"
		"                mob purge"},

	{"queue", do_mpqueue, "Queue the execution of a command.",
		"mob queue <seconds> command to run when time is up",
		"mobprog variables used in the queue commands are resolved at the time the queueing is performed... "
		"e.g. $r will be a random person at the time a prog is queue... not when it is run.  To get random at "
		"the time a prog runs, put the instructions in a prog then use the 'mq# mob call <vnum>' command.",
		"mob queue 2 north`1"
		"              mob queue 2 yell hello`1"
		"              mob queue 3 \\2n`1"
		"              mob queue 3 yell far hello`1"
		"              mob queue 4 \\3s`1"
		"              mob queue 4 yell here hello`1"
		"              mob queue 1 yell first here hello because time is 1 second`1"
		"              `=CThe mob queue #  (where # is a number in seconds) can be shortened to mq# (for example):`x`1"
		"              mq2 north`1"
		"              mq2 yell hello`1"
		"              mq3 \\2n`1"
		"              mq3 yell far hello`1"
		"              mq4 \\3s`1"
		"              mq4 yell here hello`1"
		"              mq1 yell first here hello because time is 1 second`1"
	},

	{"remember",do_mpremember, "mob will remember specified target as $Q (for future use)",
		"mob remember [target]",
		"Usually used in conjunction with another mprog",
		"  mob remember $n`1"
		"                say You have 10 seconds to leave this area`1"
		"                mob delay 5`1"
		"                `#`W(Followed by another mprog with trigger as delay)`1"
		"                (the other mprog should use $Q or $q within it to call on the remembered target)`1"},
	{"remove",do_mpremove, "causes mob to remove a certain item or group of items from the target's inventory",
		"mob remove <target> <vnum of object>",
		"'all' can be used in place of vnum to take everything from the target's inventory.  "
		"If the vnum of an object is used (mob remove $n 350) it will remove all 350's from $n. "
		"You can specify the maximum number of an object to take using the number*vnum syntax... e.g 'mob remove $n 5*350'.",
		"  emote creates a powerful vortex, sucking everything in!`1"
		"                mob remove $n all"},

	{"remort",do_remort, "Remort command to allow someone who is a hero to begin creation.",
		"mob remort <player>",
		"Note: This command only works if the mud has remort enabled in the imp gamesettings command.",
		"if level $n < 91`1"
		"                '>$n you arent ready for such a journey.`1"
		"                end`1"
		"                endif`1"
		"`1"
		"                if level $n > 91`1"
		"                  '>$n I dont have the power to rebirth an immortal`1"
		"                  end`1"
		"                endif`1"
		"                * remort the hero away :)`1"
		"                '>$n very well, as you wish.`1"
		"                ,$n points at @`1"
		"                mq2 mob echoaround $n A blinding light surrounds $N`1"
		"                mq2 mob echoat $n A blinding light engulfs you.`1"
		"                mq7 mob echoaround $n The light fade to nothingness, you notice that $N is gone.`1"
		"                mq7 mob echoat $n As the light fades you feel yourself pulls away from your old self.`1"
		"                mq8 mob remort $n"},

	{"restore",do_restore, "causes mob to restore the target as like imm restore ability",
		"mob restore <target>",
		"restores hitpoints, mana, movemen, tiredness, hunger to full",
		"  mob echo A soothing white light fills the room`1"
		"  mob restore $n"},
	{"setskill",do_mpsetskill, "Assign victim a skill/spell.",
		"mob setskill <target> <skill> <percent> [pracs] [trains]",
		"   mob setskill $n armour 10 1 0  - this will set the character's armour spell`1"
		"       at 10% at the cost of one prac.  Be aware that if you set the spell on say`1"
		"       a barbarian, they won't be able to cast it, so be sure only to assign skills/spells`1"
		"       to people of the correct character class (use if class $n to make sure.`1"
		"       [pracs] and [trains] are optional, you can just use 0 for completion's sake.`1"
		"       note: The percentage is a flexivalue (same as in in the autobalance command).`1"
		"             e.g. mob setskill $n human +15 1 0 (will add 15% to their skill, max 100%)",
		"  mob setskill $n armour 10 1 0`1"
		"             You can also use a flexivalue of +0 to change a players trains or pracs`1"
		"             without changing their skills... e.g. mob setskill $n recall +0 3 -2`1" 
		"             costs $n 3 pracs, gives 2 trains and leaves $n's recall skill unchanged."},

	{"silently",do_silently,"Will make the action unseen by characters, (same as imm command)",
		"mob silently <action>",
		"mobs will not see this as well",
		"  mob emote makes a lightning fast move with a flick of his wrist `1"
		"                mob load [key-vnum]`1"
		"                mob silently unlock east"},
	{"swipe",do_mpswipe,"Will take a certain amount of coins away from the target",
		"mob swipe <target> [gold|silver] <amount percent>"
		"  mob swipe $n gold 10 - 10% of $n's gold is gone",
		"  Note: the swipe command is 100% successful"},
	{"stun",do_mpstun, "Lags victim x pulses.",
		"mob stun <target> <1-20>",
		"There are 4 pulses per second, so 'mob stun $n 10', will lag $n 2.5 seconds.",
		"  mob stun $n 15"},
	{"tgive",do_tgive, "Used to make the mob give the target a token",
		"mob tgive <token> <target>",
		"As a general policy, use the vnum of the token to specify... it is not necessary any more that the vnum of the token is used in the name/short of the token itself.",
		"  mob tgive 3086 $n"},
	{"tjunk",do_tjunk, "Used to junk a token or group of tokens carried by the target",
		"tjunk <tokennumber> <person>  `#`Sor`&  tjunk all.<tokennumber> <person>",
		"Prefixing the token number with all. will clear all tokens of that vnum from the target.  "
		"Examples of the target might be the mob itself ($I) or another player ($N))",
		"  mob tjunk all.3086 $n`1"
		"                mob tjunk 4236 $n"},
	{"transfer",do_mptransfer, "Used to transfer a single person or all players in a room to a certain location",
		"mob transfer <target|'all'> [location]",
		"The 'all' is used to transfer everyone in the current room to the location specified.  "
		"The mob doesn't have to be in the room of the target... if no location is specified the "
		"target will be transfered into the current room",
		"  mob transfer badplayer 299`1"
		"                mob transfer all 3000"},
	{"tremove",do_tremove, "Takes a token from a player",
		"mob tremove <token> <player>",
		"It is prefered that the token is referred by its vnum.  "
		"tremove will put the token into the mobs inventory.  If you are wanting to "
		"destroy the token it is easier to use tjunk",
		"no example, because there is little need for this since all token editing commands can be done when the player is holding a token."
	},
	{"ttimer",do_ttimer, "Sets the number of ticks in which the token will automatical dissolve",
		"mob ttimer <token> <player> <number_of_ticks>",
		"It is prefered that the token is referred by its vnum.  ",
		"  mob ttimer 3086 $n 20 * in twenty ticks this will dissolve."	
	},

	{"wander",do_mpwander, "Used to make a mob walk in a random direction.",
		"mob wander"
	},

	{"xpreward",do_mpxpreward, "Reward victim with xp.",
		"mob xpreward <target> <amount>",
		"   mob xpreward $n 50  - this will give the character 50 xp`1"
		"   mob xpreward $n -10  - this will take 10 xp from the character.`1"
		"   notes: The character must be in the same room as the mob.`1"
		"          No message is shown to the character unless they gain a level.`1"
		"          You can't lose a level with this command, but can go into negative xp.`1"
		"          You can't set more than 1000 xp at a time.`1"
		"          The system logs the affect of this command."
		},

	{"vforce",do_mpvforce, "Same as force but uses vnum to specify target instead",
		"mob vforce <vnum> <command>",
		"Very useful if you know there will be only 1 mob with a particular vnum in the realm.  "
		"See the force command for additional notes (mobhelp force)"},

	{	"zecho",		do_mpzecho,
		"Zone Echo - Prints the string to all active players in the same area/zone as the mob",
		"mob zecho <string>",
		"same as immortal zecho command"},
	{	"zoecho",		do_mpzoecho,
		"Zone Outdoor Echo - Prints the string to all active players in the same area/zone as the mob if they are outdoors", 
		"mob zoecho <string>",
		"same as immortal zoecho command"},
	{	"zuecho",		do_mpzuecho,
		"Zone Underwater Echo - Prints the string to all active players in the same area/zone as the mob if they are underwater",
		"mob zuecho <string>",
		"similar to all zone echo commands"},
	{	"",				0, "", "" }
};

/**************************************************************************/
void do_mob( char_data *ch, char *argument )
{
	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if ( ch->master ){
			ch->master->println( "Not going to happen.");
		}
		return;
	}

	// Security check!
    if ( ch->desc != NULL && get_trust(ch) < MAX_LEVEL ){
		do_huh(ch,"");
		return;
	}
    mob_interpret( ch, argument );
}
/**************************************************************************/
void do_mpqueue(char_data *ch, char *argument);
/**************************************************************************/
void do_mobqueue( char_data *ch, char *argument )
{
	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if ( ch->master ){
			ch->master->println( "Not going to happen.");
		}
		return;
	}

	// Security check!
    if ( ch->desc != NULL && get_trust(ch) < LEVEL_IMMORTAL ){
		do_huh(ch,"");
		return;
	}
	do_mpqueue(ch, argument); 
}
/**************************************************************************/
// a command to display all the mob commands
void do_mobhelp( char_data *ch, char *argument )
{
    int cmd;
	bool found=false;
	// do the checks to make sure they can use the command here
	if (!HAS_SECURITY(ch,1))
	{
		ch->println("The mobhelp command is an olc mobprog programming related command, "
			"you dont have olc permissions.");
		return;
	}

    if ( !HAS_SECURITY(ch, MPEDIT_MINSECURITY))
    {
    	ch->printlnf("mobhelp: Insufficient olc security to use this command %d required.", 
			MPEDIT_MINSECURITY);
    	return;
    }
	
	if(IS_NULLSTR(argument)){
		// command list syntax
		for ( cmd = 0; !IS_NULLSTR(mob_cmd_table[cmd].name); cmd++ )
		{
			ch->printlnf("`W%-11s `S- `B%s",
				capitalize(mob_cmd_table[cmd].name),
				IS_NULLSTR(mob_cmd_table[cmd].descript)?"`Sno descripton":mob_cmd_table[cmd].descript);
		}
		ch->println("`WTo see more info on a given command, type `=Cmobhelp <command>`x");
		ch->println("`WParameters surrounded with <> are required, [] are optional parameters.`x");
	}else{
		// specific list syntax
		for ( cmd = 0; !IS_NULLSTR(mob_cmd_table[cmd].name); cmd++ )
		{
			// filter in only required commands
			if(str_infix(argument,mob_cmd_table[cmd].name))
				continue;
			ch->printlnf("`W%-11s `S- `B%s\r\n"
				"      `Ssyntax: `=C%s\r\n"
				"      `Snotes:  `g%s\r\n"
				"      `Sexample:`x%s\r\n", 
				capitalize(mob_cmd_table[cmd].name),
				IS_NULLSTR(mob_cmd_table[cmd].descript)?"`Sno descripton":mob_cmd_table[cmd].descript,
				IS_NULLSTR(mob_cmd_table[cmd].syntax)?"`Sno syntax":mob_cmd_table[cmd].syntax,
				IS_NULLSTR(mob_cmd_table[cmd].notes)?"`Sno notes":mob_cmd_table[cmd].notes,
				IS_NULLSTR(mob_cmd_table[cmd].example)?"`Sno example":mob_cmd_table[cmd].example);
			found=true;
		}
		if(!found){
			ch->printlnf("There are no mob commands with the text '%s' within.", argument);
		}
	}
}
/**************************************************************************/
/*
 * Mob command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void mob_interpret( char_data *ch, char *argument )
{
    char command[MIL];
    int cmd;
	
	// Record the moblog command
	if( IS_NPC(ch) && IS_SET(ch->act,ACT_MOBLOG) ){
		append_timestring_to_file( MOBLOG_LOGFILE, 
			FORMATF("[%5d] in room %d mob_interpret '%s'", 
			(ch->pIndexData?ch->pIndexData->vnum:0),
			(ch->in_room? ch->in_room->vnum:0),
			argument));
	}

    argument = one_argument( argument, command );

    // Look for command in command table.
    for ( cmd = 0; mob_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
		if ( command[0] == mob_cmd_table[cmd].name[0]
			&&   !str_prefix( command, mob_cmd_table[cmd].name ) )
		{
			SET_BIT(ch->dyn,DYN_RUNNING_MOBPROG_CMD);
			(*mob_cmd_table[cmd].do_fun) ( ch, argument );
			REMOVE_BIT(ch->dyn,DYN_RUNNING_MOBPROG_CMD);
			tail_chain( );
			return;
		}
    }

	// Record the moblog command
	if( IS_NPC(ch) && IS_SET(ch->act,ACT_MOBLOG) ){
		append_timestring_to_file( MOBLOG_LOGFILE, 
			FORMATF("[%5d] in room %d mob_interpret unfound command '%s'", 
			(ch->pIndexData?ch->pIndexData->vnum:0),
			(ch->in_room? ch->in_room->vnum:0),
			command));
	}

    mpbug( "Mob_interpret: unrecognised mob command '%s'", command );
}

/**************************************************************************/
char *mprog_type_to_name( int type )
{
    switch ( type )
    {
    case TRIG_ACT:			return "ACT";
    case TRIG_SPEECH:		return "SPEECH";
    case TRIG_RANDOM:		return "RANDOM";
    case TRIG_FIGHT:		return "FIGHT";
    case TRIG_HPCNT:		return "HPCNT";
    case TRIG_DEATH:		return "DEATH";
    case TRIG_ENTRY:		return "ENTRY";
    case TRIG_GREET:		return "GREET";
    case TRIG_GRALL:		return "GRALL";
    case TRIG_GIVE:			return "GIVE";
    case TRIG_BRIBE:		return "BRIBE";
    case TRIG_KILL:			return "KILL";
    case TRIG_DELAY:		return "DELAY";
    case TRIG_SURR:			return "SURRENDER";
    case TRIG_EXIT:			return "EXIT";
    case TRIG_EXALL:		return "EXALL";
	case TRIG_REPOP:		return "REPOP";
	case TRIG_ROOMDEATH:	return "ROOMDEATH";
	case TRIG_TICK:			return "TICK";
	case TRIG_HOUR:			return "HOUR";
	case TRIG_COMMAND:		return "COMMAND";
	case TRIG_PREKILL:		return "PREKILL";
	case TRIG_SAYTO:		return "SAYTO";

	case TRIG_LOGINROOM:	return "LOGINROOM";
	case TRIG_LOGINAREA:	return "LOGINAREA";
	case TRIG_LOGOUTROOM:	return "LOGOUTROOM";
	case TRIG_LOGOUTAREA:	return "LOGOUTAREA";
	case TRIG_PRETRAIN:		return "PRETRAIN";
	case TRIG_PREPRAC:		return "PREPRAC";

	case 0:					return "CALLED_FROM_PREVIOUS";
    default:				return "ERROR";
    }
}
/**************************************************************************/
/* 
 * Displays MOBprogram triggers of a mobile
 *
 * Syntax: mpstat [name]
 */
void do_mpstat( char_data *ch, char *argument )
{
    char        arg[ MSL  ];
    MPROG_LIST  *mprg;
    char_data   *victim;
    int i;

    one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		ch->println("Mpstat whom?");
		return;
	}

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		ch->println("No such creature.");
		return;
	}

    if ( !IS_NPC( victim ) )
	{
		ch->println("That is not a mobile.");
		return;
	}

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		ch->println("No such creature visible.");
		return;
	}
	
    ch->printlnf("Mobile #%-6d [%s]",
		victim->pIndexData->vnum, victim->short_descr );

	ch->printlnf("Delay   %-6d [%s]",
		victim->mprog_delay,
		victim->mprog_target == NULL 
		? "No target" : victim->mprog_target->name );
	
    if ( !victim->pIndexData->mprog_flags )
    {
		ch->println("[No programs set]");
		return;
    }
	
    for ( i = 0, mprg = victim->pIndexData->mprogs; mprg != NULL;
	mprg = mprg->next )
		
    {
		ch->printlnf("[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]",
			++i,
			mprog_type_to_name( mprg->trig_type ),
			mprg->prog->vnum,
			mprg->trig_phrase );
    }
	return;
}
/**************************************************************************/
/*
 * Displays the source code of a given MOBprogram
 *
 * Syntax: mpdump [vnum] full
 */
void do_mpdump( char_data *ch, char *argument )
{
	char buf[MIL];
	MPROG_CODE *mprg;

	argument =one_argument( argument, buf );
	if ( ( mprg = get_mprog_index( atoi(buf) ) ) == NULL )
	{
		ch->println("No such MOBprogram.");
		return;
	}
	ch->printlnf("`y   -=== `YMobprog %d - `r%s `y===-", 
		mprg->vnum, 
	   (IS_NULLSTR(mprg->title)?"`Sunknown":mprg->title));
	ch->sendpage("`x\r\n");
	ch->sendpage(mprg->code);
	ch->sendpage("`x\r\n");

	if (!IS_NULLSTR(argument) && !str_prefix(argument,"showmobs"))
	{
		MOB_INDEX_DATA *mob;
		int hash;
		MPROG_LIST *mpl;

		for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
		{
			for ( mob = mob_index_hash[hash]; mob; mob = mob->next )
			{
				for ( mpl = mob->mprogs; mpl; mpl = mpl->next )
				{
					if ( mpl->prog == mprg )
					{
						ch->printlnf("%d %s (%s) uses prog %d.", 
							mob->vnum, mob->short_descr, 
							mob->player_name, mprg->vnum);
					}
				}
			}
		}
	}
	else
	{
		ch->printf("To see what mobs are using this prog type "
			"`=Cmpdump %s showmobs`x\r\n", buf);
	}

}
/**************************************************************************/
/*
 * Prints the argument to all active players in the game
 *
 * Syntax: mob gecho [string]
 */
void do_mpgecho( char_data *ch, char *argument )
{
    connection_data *d;

    if ( argument[0] == '\0' )
    {
	bugf( "MpGEcho: missing argument from vnum %d",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    for ( d = connection_list; d; d = d->next )
    {
		if ( d->connected_state == CON_PLAYING )
		{
			if ( IS_IMMORTAL(d->character) ){
				d->character->printf(
					"Mob %d (prog%d) gecho> ", 
						ch->pIndexData?ch->pIndexData->vnum:0,
						call_level>0?callstack_pvnum[call_level-1]:0);
			}
			d->character->println( argument);
		}
    }
}
/**************************************************************************/
/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: mob zecho [string]
 */
void do_mpzecho( char_data *ch, char *argument )
{
    connection_data *d;
	
    if ( argument[0] == '\0' )
    {
		mpbug( "MpZEcho: missing argument from vnum %d",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
    }
	
    if ( ch->in_room == NULL )
		return;
	
    for ( d = connection_list; d; d = d->next )
    {
		if ( d->connected_state == CON_PLAYING 
			&&   d->character->in_room != NULL 
			&&   d->character->in_room->area == ch->in_room->area )
		{
			if ( IS_IMMORTAL(d->character) ){
				d->character->printf(
					"Mob %d (prog%d) zecho> ", 
						ch->pIndexData?ch->pIndexData->vnum:0,
						call_level>0?callstack_pvnum[call_level-1]:0);
			}
			d->character->println( argument);
		}
    }
}
/**************************************************************************/
void do_mpzoecho( char_data *ch, char *argument )
{
    connection_data *d;
    
    if ( argument[0] == '\0' )
    {
		mpbug( "MpZOEcho: missing argument from vnum %d",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
    }

	if ( ch->in_room == NULL )
		return;

    for ( d = connection_list; d; d = d->next )
    {
		if(d->connected_state == CON_PLAYING ){
			if( IS_OUTSIDE(d->character)
			&&	 d->character->in_room->sector_type != SECT_CAVE
			&&   IS_AWAKE(d->character)
			&&	 d->character->in_room != NULL
			&&   ch->in_room != NULL
			&&   d->character->in_room->area == ch->in_room->area )
			{
				if(IS_IMMORTAL(d->character)){
					d->character->print( "zoneoutdoors> ");
				}
				d->character->printf("%s`x\r\n", argument );
			}
		}
	}

    return;
}

/**************************************************************************/
/*
 * debug zone echo
 * Prints the argument to all builders/imms in the same area as the mob
 * 
 * Syntax: mob dzecho [string]
 */
void do_mpdzecho( char_data *ch, char *argument )
{
    connection_data *d;
	
    if ( argument[0] == '\0' )
    {
		mpbug( "MpDZEcho: missing argument from vnum %d",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
    }
	
    if ( ch->in_room == NULL )
		return;
	
    for ( d = connection_list; d; d = d->next )
    {
		if ( d->connected_state == CON_PLAYING 
			&&   d->character->in_room != NULL 
			&&   d->character->in_room->area == ch->in_room->area )
		{
			if ( IS_IMMORTAL(d->character) 
				|| IS_SET(d->character->comm, COMM_BUILDING)){
				d->character->printf(
					"Mob %d in room %d, (prog%d) DEBUG ZONE ECHO> ", 
						ch->pIndexData?ch->pIndexData->vnum:0,
						ch->in_room->vnum,
						call_level>0?callstack_pvnum[call_level-1]:0);				
				d->character->println( argument);
			}
		}
    }
}
/**************************************************************************/
/*
 * Prints the argument to all the rooms around the mobile
 *
 * Syntax: mob asound [string]
 */
void do_mpasound( char_data *ch, char *argument )
{
	
    ROOM_INDEX_DATA *was_in_room;
    int              door;
	
    if ( argument[0] == '\0' )
		return;
	
    was_in_room = ch->in_room;
    for ( door = 0; door < MAX_DIR; door++ )
    {
		EXIT_DATA       *pexit;
		
		if ( ( pexit = was_in_room->exit[door] ) != NULL
			&&   pexit->u1.to_room != NULL
			&&   pexit->u1.to_room != was_in_room )
		{
			ch->in_room = pexit->u1.to_room;
			MOBtrigger  = false;
			act( argument, ch, NULL, NULL, TO_ROOM );
			MOBtrigger  = true;
		}
    }
    ch->in_room = was_in_room;
    return;
	
}
/**************************************************************************/
/*
 * Lets the mobile kill any player or mobile without murder
 *
 * Syntax: mob kill [victim]
 */
void do_mpkill( char_data *ch, char *argument )
{
    char      arg[ MIL ];
    char_data *victim;
	
    one_argument( argument, arg );
	
    if ( arg[0] == '\0' )
		return;
	
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
		return;
	
    if ( victim == ch || IS_NPC(victim) || ch->position == POS_FIGHTING )
		return;
	
    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
		bugf( "MpKill - Charmed mob attacking master from vnum %d.",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
    }
	
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}
/**************************************************************************/
/*
 * Lets the mobile assist another mob or player
 *
 * Syntax: mob assist [character]
 */
void do_mpassist( char_data *ch, char *argument )
{
    char      arg[ MIL ];
    char_data *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	return;

    if ( victim == ch || ch->fighting != NULL || victim->fighting == NULL )
	return;

    multi_hit( ch, victim->fighting, TYPE_UNDEFINED );
    return;
}
/**************************************************************************/
/*
 * Lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy 
 * items using all.xxxxx or just plain all of them 
 *
 * Syntax: mob junk [item]
 */
void do_mpjunk( char_data *ch, char *argument )
{
    char      arg[ MIL ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
	
    one_argument( argument, arg );
	
    if ( arg[0] == '\0')
		return;
	
    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
		if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
		{
			unequip_char( ch, obj );
			extract_obj( obj );
			return;
		}
		if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
			return; 
		extract_obj( obj );
    }else{
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
            obj_next = obj->next_content;
			if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            {
				if ( obj->wear_loc != WEAR_NONE)
					unequip_char( ch, obj );
				extract_obj( obj );
            } 
		}
	}	
	return;
		
}
/**************************************************************************/
/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: mob echoaround [victim] [string]
 */
void do_mpechoaround( char_data *ch, char *argument )
{
    char       arg[ MIL ];
    char_data *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( ch, arg ) ) == NULL )
	return;

    act( argument, ch, NULL, victim, TO_NOTVICT );
}
/**************************************************************************/
/*
 * Prints the message to only the victim
 *
 * Syntax: mob echoat [victim] [string]
 */
void do_mpechoat( char_data *ch, char *argument )
{
    char       arg[ MIL ];
    char_data *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	return;

    act( argument, ch, NULL, victim, TO_VICT );
}
/**************************************************************************/
/*
 * Prints the message to the room at large
 *
 * Syntax: mpecho [string]
 */

void do_mpecho( char_data *ch, char *argument )
{
    if ( argument[0] == '\0' )
	return;
    act( argument, ch, NULL, NULL, TO_ROOM );
}
/**************************************************************************/
/*
 * Lets the mobile load another mobile.
 *
 * Syntax: mob mload [vnum]
 */
void do_mpmload( char_data *ch, char *argument )
{
    char            arg[ MIL ];
    MOB_INDEX_DATA *pMobIndex;
    char_data      *victim;
    vn_int vnum;

    one_argument( argument, arg );

    if ( ch->in_room == NULL || arg[0] == '\0' || !is_number(arg) )
	return;

	if(!is_number(arg)){
		mpbug( arg, "Mpmload: single arg needs to be the mob vnum to load - '%s' invalid!",	arg);
		return;
	}
    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
		mpbug( arg, "Mpmload: mob vnum %d not found to load!",	atoi(arg));
		return;
    }
    victim = create_mobile( pMobIndex, 0 );
    char_to_room( victim, ch->in_room );
    return;
}
/**************************************************************************/
/*
 * Lets the mobile load an object
 *
 * Syntax: mob oload <vnum> [level] [R|W]
 */
void do_mpoload( char_data *ch, char *argument )
{
    char arg1[ MIL ];
    char arg2[ MIL ];
    char arg3[ MIL ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;
    bool            fToroom = false, fWear = false;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );
 
    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        mpbug( "Mpoload - arg1 is nonnumeric '%s' (should be vnum to load).",
			arg1);
        return;
    }
 
    if ( IS_NULLSTR(arg2))
    {
		level = get_trust( ch );
    }
    else
    {
        if ( !is_number( arg2 ) )
        {
			mpbug( "Mpoload - optional arg2 is nonnumeric '%s' (should be level to load).",
				arg2);
			return;
        }
		level = atoi( arg2 );
		if ( level < 0 || level > get_trust( ch ) )
		{
			mpbug( "Mpoload - optional arg2 is an invalid level '%s' (should be level to load, can't be greater than trust of mob).",
				arg2);
			return;
		}
    }

    /*
     * Added 3rd argument
     * omitted - load to mobile's inventory
     * 'R'     - load to room
     * 'W'     - load to mobile and force wear
     */
    if ( arg3[0] == 'R' || arg3[0] == 'r' ){
		fToroom = true;
    }else if ( arg3[0] == 'W' || arg3[0] == 'w' ){
		fWear = true;
	}

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
        mpbug( "Mpoload - object for arg1 %d not found!", atoi(arg1));
		return;
    }

    obj = create_object( pObjIndex);

	// all tokens are 'takeable' 
	if(obj->item_type==ITEM_TOKEN){
		SET_BIT(obj->wear_flags, OBJWEAR_TAKE);
	}

    if ( (fWear || !fToroom) && CAN_WEAR(obj, OBJWEAR_TAKE) )
    {
		obj_to_char( obj, ch );
		if ( fWear ){
			wear_obj( ch, obj, true, false );
		}
    }else{
		obj_to_room( obj, ch->in_room );
    }
    return;
}
/**************************************************************************/
/*
 * Lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room. The mobile cannot
 * purge itself for safety reasons.
 *
 * syntax mob purge {target}
 */
void do_mppurge( char_data *ch, char *argument )
{
    char       arg[ MIL ];
    char_data *victim;
    OBJ_DATA  *obj;

	logf("do_mppurge( %s [%d], %s)", ch->name,ch->vnum(), argument);

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        // 'purge' 
        char_data *vnext;
        OBJ_DATA  *obj_next;
		
		for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
		{
			vnext = victim->next_in_room;
			if ( IS_NPC( victim ) && victim != ch 
				&&   !IS_SET(victim->act, ACT_NOPURGE) )
				extract_char( victim, true );
		}
		
		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( !IS_SET(obj->extra_flags, OBJEXTRA_NOPURGE) )
				extract_obj( obj );
		}
		
		return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
		if ( ( obj = get_obj_here( ch, arg ) ) )
		{
			extract_obj( obj );
		}
		else
		{
			mpbug( "Mppurge - object specified '%s' not found.", arg);
		}
		return;
    }

    if ( !IS_NPC( victim ) )
    {
		mpbug( "Mppurge - attempting to purging a PC - not accepted.");			
		return;
    }
    extract_char( victim, true );
    return;
}
/**************************************************************************/
/*
 * Lets the mobile goto any location it wishes that is not private.
 *
 * Syntax: mob goto [location]
 */
void do_mpgoto( char_data *ch, char *argument )
{
    char             arg[ MIL ];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
		mpbug( "Mpgoto - No argument!");
		return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
		mpbug( "Mpgoto - No such location '%s'.", arg);
		return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, true );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}
/**************************************************************************/
/* 
 * Lets the mobile do a command at another location.
 *
 * Syntax: mob at [location] [commands]
 */
void do_mpat( char_data *ch, char *argument )
{
    char             arg[ MIL ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    char_data       *wch;
    OBJ_DATA 	    *on;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
		mpbug( "Mpat - 2 args required minimum");
		return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
		mpbug( "Mpat - No such location '%s'",arg);
		return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = on;
	    break;
	}
    }

    return;
}
/**************************************************************************/ 
/*
 * Lets the mobile transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: mob transfer [target|'all'] [location]
 */
void do_mptransfer( char_data *ch, char *argument )
{
    char             arg1[ MIL ];
    char             arg2[ MIL ];
    char	     buf[MSL];
    ROOM_INDEX_DATA *location;
    char_data       *victim;
	
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
	
    if ( IS_NULLSTR(arg1) )
    {
		mpbug( "Mptransfer - needs more arguments, Syntax: mob transfer [target|'all'] [location]");
		return;
    }
	
    if ( !str_cmp( arg1, "all" ) )
    {
		char_data *victim_next;
		
		for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( !IS_NPC(victim) )
			{
				sprintf( buf, "%s %s", victim->name, arg2 );
				do_mptransfer( ch, buf );
			}
		}
		return;
    }

	// first check in the room for the player in the room
	if ( ( victim = get_char_room( ch, arg1 ) ) == NULL ){
		if ( ( victim = get_char_world( ch, arg1 ) ) == NULL ){
			mpbug("do_mptransfer() Couldn't find any '%s' in the room (%d) or realm.", 
				arg1, victim->in_room_vnum());
			return;
		}
	}
	
    /*
	* Thanks to Grodyn for the optional location parameter.
	*/
    if ( IS_NULLSTR(arg2) )
    {
		location = ch->in_room;
    }
    else
    {
		if ( ( location = find_location( ch, arg2 ) ) == NULL )
		{
			mpbug( "Mptransfer - arg2 location '%s' not found.",arg2);
			return;
		}
		
		if ( is_room_private_to_char( location, ch) ){
			mpbug( "Mptransfer - trying to transfer '%s' to a private room %d.",
				arg1, location->vnum);
			ch->printlnf("Room %d is private right now.", location->vnum);
			return;			
		}
    }
	
    if ( victim->in_room == NULL )
		return;
	
    if ( victim->fighting != NULL ){
		stop_fighting( victim, true );
	}
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );
	
    return;
}
/**************************************************************************/
/*
 * Lets the mobile transfer all chars in same group as the victim.
 *
 * Syntax: mob gtransfer [victim] [location]
 */
void do_mpgtransfer( char_data *ch, char *argument )
{
    char             arg1[ MIL ];
    char             arg2[ MIL ];
    char	     buf[MSL];
    char_data       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
		mpbug( "Mpgtransfer - arg1 (victim) missing!");		
		return;
    }

    if ( (who = get_char_room( ch, arg1 )) == NULL )
	return;

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
		    sprintf( buf, "%s %s", victim->name, arg2 );
		    do_mptransfer( ch, buf );
    	}
    }
    return;
}
/**************************************************************************/
/*
 * Lets the mobile force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the mobile.
 *
 * Syntax: mob force [victim] [commands]
 */
void do_mpforce( char_data *ch, char *argument )
{
    char arg[ MIL ];
	
    argument = one_argument( argument, arg );
	
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
		mpbug( "Mpforce - 2 args required.");			
		return;
    }
	
    if ( !str_cmp( arg, "all" ) )
    {
        char_data *vch;
        char_data *vch_next;
		
		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;
			
			if ( vch->in_room == ch->in_room
				&& get_trust( vch ) < get_trust( ch ) 
				&& can_see( ch, vch ) )
			{
				if ( IS_IMMORTAL( vch ))
				{
					vch->printlnf("Mob %d running prog %d attempted to force you to '%s'... not done because you are an immortal.",
						ch->vnum(), callstack_pvnum[call_level], argument);
					continue;
				}
				interpret( vch, argument );
			}
		}
    }
    else
    {
		char_data *victim;
		
		if ( ( victim = get_char_room( ch, arg ) ) == NULL ){
			mpbug("mpforce: couldn't find victim '%s' in room %d",
				arg, ch->in_room?ch->in_room->vnum:0);
			return;
		}
		
		if ( victim == ch ){
			mpbug("mpforce: found self as victim using '%s' in room %d",
				arg, ch->in_room?ch->in_room->vnum:0);
			return;
		}	
		if ( IS_IMMORTAL( victim ))
		{
			victim->printlnf("Mob %d running prog %d attempted to force you to '%s'... not done because you are an immortal.",
				ch->vnum(), callstack_pvnum[call_level], argument);
			return;
		}
		interpret( victim, argument );
    }
	
    return;
}
/**************************************************************************/
/*
 * Lets the mobile force a group something. Must be mortal level.
 *
 * Syntax: mob gforce [victim] [commands]
 */
void do_mpgforce( char_data *ch, char *argument )
{
	char arg[ MIL ];
	char_data *victim, *vch, *vch_next;
	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		mpbug( "MpGforce - 2 args required");
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
		return;

	if ( victim == ch )
		return;

	for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
	{
		vch_next = vch->next_in_room;
		
		if ( is_same_group(victim,vch) )
		{
			if ( !IS_IMMORTAL( vch ))
				interpret( vch, argument );
		}
	}
	return;
}
/**************************************************************************/
/*
 * Forces all mobiles of certain vnum to do something (except ch)
 *
 * Syntax: mob vforce [vnum] [commands]
 */
void do_mpvforce( char_data *ch, char *argument )
{
    char_data *victim, *victim_next;
    char arg[ MIL ];
    vn_int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
		mpbug( "MpVforce - needs 2 args.");
		return;
    }

    if ( !is_number( arg ) )
    {
		mpbug( "MpVforce - Non-number argument '%s'.", arg);		
		return;
    }

    vnum = atoi( arg );

	if(IS_SET(ch->dyn,DYN_IN_ROOM_ONLY)){
		for ( victim = ch->in_room->people; victim; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
				&&   ch != victim && victim->fighting == NULL )
			{
				interpret( victim, argument );
			}
		}
	}else{
		for ( victim = char_list; victim; victim = victim_next )
		{
			victim_next = victim->next;
			if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
				&&   ch != victim && victim->fighting == NULL )
			{
				interpret( victim, argument );
			}
		}
	}
    return;
}
/**************************************************************************/
/*
 * Lets the mobile cast spells --
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: mob cast [spell] {target}
 */
// - a lot of checking and bug fixes by Kal, May 98
void do_mpcast( char_data *ch, char *argument )
{
    char_data *victim;
    OBJ_DATA *obj;
    void *vo= NULL;
    char spell[MIL], tarbuf[MIL];
    int target;
    int sn;

    argument = one_argument( argument, spell );
               one_argument( argument, tarbuf );

    if ( spell[0] == '\0' )
    {
		mpbug("MpCast - no arguments specified, at least spell must be set!");
		return;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    {
		mpbug("MpCast - No such spell '%s'", spell);
		return;
    }

    victim = get_char_room( ch, tarbuf );
    obj = get_obj_here( ch, tarbuf );

    target = TARGET_NONE;
    switch ( skill_table[sn].target )
    {
		default: 
			mpbug( "MpCast - bad cast target type, (spell='%s',sn=%d, skill_table[sn].target=%d).", 
				spell, sn, skill_table[sn].target);
			return;	

		case TAR_IGNORE: 
			break;
		
	    case TAR_MOB_OFFENSIVE:
		case TAR_CHAR_OFFENSIVE: 
			if ( !victim || victim== ch ){
				if(!victim){
					mpbug( "MpCast - couldn't find victim '%s', (spell='%s',sn=%d, skill_table[sn].target=%d).", 
						tarbuf, spell, sn, skill_table[sn].target);
				}
				return;
			}
			vo = ( void * ) victim;
			target = TARGET_CHAR;
		    break;
	
		case TAR_CHAR_DEFENSIVE:
			vo = victim?(void *)victim:(void *)ch; 
			target = TARGET_CHAR;
			break;

		case TAR_CHAR_SELF:
			vo = (void *) ch; 
	        target = TARGET_CHAR;
			break;

		case TAR_OBJ_INV:
			if ( !obj ){
				mpbug( "MpCast - couldn't find object '%s', (spell='%s',sn=%d, skill_table[sn].target=%d).", 
					tarbuf, spell, sn, skill_table[sn].target);
				return;
			}
			vo = (void *)obj;
	        target = TARGET_OBJ;
			break;

	    case TAR_OBJ_MOB_OFF:
		case TAR_OBJ_CHAR_OFF:
			if (IS_NULLSTR(tarbuf))
			{
				if ((victim = ch->fighting) == NULL)
				{
					//ch->println("Cast the spell on whom or what?");
					return;
				}
    			target = TARGET_CHAR;
			}
			else if ((vo = victim) != NULL)
			{
				target = TARGET_CHAR;
			}
    
			if (target == TARGET_CHAR) // check the sanity of the attack
			{
				if(is_safe_spell(ch, victim, false) && victim!= ch)
				{
					mpbug( "MpCast - Casting '%s' on a safe target '%s'", 
						spell, victim->name);
					return;
				}       
				vo = (void *) victim;
			}
			else if (obj)
			{
				vo = (void *) obj;
				target = TARGET_OBJ;
			}
			else
			{
				mpbug( "MpCast - Casting '%s' couldnt find '%s'.", 	spell, tarbuf);
				return;
			}
			break; 
    
		case TAR_OBJ_CHAR_DEF:
			if ( IS_NULLSTR(tarbuf))
			{
				vo = (void *)ch;
				target = TARGET_CHAR;
			}
			else if (victim)
			{
				vo = (void *)victim;
				target = TARGET_CHAR;
			}
			else if ((obj = get_obj_carry(ch,tarbuf)) != NULL)
			{
				vo = (void *) obj;
				target = TARGET_OBJ;
			} 
			else 
			{
				mpbug( "MpCast - Casting '%s' couldnt find '%s'"
					"(check mob has object in its inventory)", 	spell, tarbuf);
				return;
			}
			break;
    }
    	
	(*skill_table[sn].spell_fun)( sn, ch->level, ch, vo,target);
    return;
}
/**************************************************************************/
/*
 * Lets mob cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob damage [victim] [min] [max] {kill}
 */
void do_mpdamage( char_data *ch, char *argument )
{
    char_data *victim = NULL, *victim_next;
    char target[ MIL ],
	 min[ MIL ],
	 max[ MIL ];
    int low, high;
    bool fAll = false, fKill = false;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
		mpbug("MpDamage - empty target field '%s'", target);
		return;
    }
    if( !str_cmp( target, "all" ) ){
		fAll = true;
    }else{
		if( ( victim = get_char_room( ch, target ) ) == NULL ){
			return;
		}
	}

    if ( is_number( min ) ){
		low = atoi( min );
    }else{
		mpbug("MpDamage - Bad damage min amount '%s' - must be numeric.", min);
		return;
	}

    if ( is_number( max ) ){
		high = atoi( max );
    }else{
		mpbug("MpDamage - Bad damage max amount '%s' - must be numeric.", max);
		return;
    }
    one_argument( argument, target );

    // If kill parameter is omitted, this command is "safe" and will not
    // kill the victim.
    if ( target[0] != '\0' ){
		fKill = true;
	}

    if ( fAll )
    {
		for( victim = ch->in_room->people; victim; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( victim != ch )
    			damage( victim, victim, 
				fKill ? 
				number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
				TYPE_UNDEFINED, DAM_NONE, false );
		}
    }else{
    	damage( victim, victim, 
			fKill ? number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
			TYPE_UNDEFINED, DAM_NONE, false );
	}
    return;
}
/**************************************************************************/
/*
 * Lets the mobile to remember a target. The target can be referred to
 * with $q and $Q codes in MOBprograms. See also "mob forget".
 *
 * Syntax: mob remember [victim]
 */
void do_mpremember( char_data *ch, char *argument )
{
    char arg[ MIL ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' ){
		ch->mprog_target = get_char_world( ch, arg );
		if(!ch->mprog_target){ 
			mpbug( "MpRemember: remember target '%s' not found.", arg);
		}
    }else{
		mpbug( "MpRemember: argument '%s' is invalid.", arg);
	}
}
/**************************************************************************/
/*
 * Reverse of "mob remember".
 *
 * Syntax: mob forget
 */
void do_mpforget( char_data *ch, char *)
{
    ch->mprog_target = NULL;
}
/**************************************************************************/
/*
 * Sets a delay for MOBprogram execution. When the delay time expires,
 * the mobile is checked for a MObprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_MOBILE
 *
 * Syntax: mob delay [pulses]
 */
void do_mpdelay( char_data *ch, char *argument )
{
    char arg[ MIL ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
		mpbug( "MpDelay: invalid arg '%s' - must be a numeric value in "
			"pulses (there are %d per second).", arg, PULSE_PER_SECOND);
		return;
    }
    ch->mprog_delay = atoi( arg );
}
/**************************************************************************/
/*
 * Reverse of "mob delay", deactivates the timer.
 *
 * Syntax: mob cancel
 */
void do_mpcancel( char_data *ch, char *)
{
   ch->mprog_delay = -1;
}
/**************************************************************************/
/*
 * Lets the mobile to call another MOBprogram withing a MOBprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: mob call <vnum> [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_mpcall( char_data *ch, char *argument )
{
    char arg1[MIL], arg[MIL];
    char_data *vch;
    OBJ_DATA *obj1, *obj2;
    MPROG_CODE *prg;
//    extern void program_flow( sh_int, char *, char_data *, char_data *, const void *, const void * );

    argument = one_argument( argument, arg1 );
    if ( IS_NULLSTR(arg1))
    {
		mpbug( "MpCall: no arguments!, need at least the mprog vnum to run (arg1), usually $n is second arg (victim), arg3 is object1, arg4 is object2.");
		mpbug( "Most common usuage is like 'mob call <mprognum> $n'");
		return;
    }
    if ( ( prg = get_mprog_index( atoi(arg1) ) ) == NULL )
    {
		mpbug( "MpCall: arg1 '%s' wasnt found, arg1 must be the mobprog vnum of the program to run.", arg1);
		return;
    }
	// default values
    vch = NULL;
    obj1 = NULL;
	obj2 = NULL;

    argument = one_argument( argument, arg );
    if ( !IS_NULLSTR(arg)){
        vch = get_char_room( ch, arg );
        if(!vch){
			mpbug( "MpCall: victim arg2 '%s' wasnt found in room to hand over to called function %d.", 
				arg, atoi(arg1));
		}
	}

    argument = one_argument( argument, arg );
    if ( !IS_NULLSTR(arg)){
    	obj1 = get_obj_here( ch, arg );
		if(!obj1){
			mpbug( "MpCall: obj1 arg3 '%s' wasnt found in room to hand over to called function %d.", 
				arg, atoi(arg1));
		}
	}


    argument = one_argument( argument, arg );
    if ( !IS_NULLSTR(arg)){
    	obj2 = get_obj_here( ch, arg );
		if(!obj2){
			mpbug( "MpCall: obj2 arg4 '%s' wasnt found in room to hand over to called function %d.", 
				arg, atoi(arg1));
		}
	}
	MPROG_LIST mproglist_item;
	mproglist_item.prog=prg;
	mproglist_item.next=NULL;
	mproglist_item.trig_phrase="";
	mproglist_item.trig_type=0;

	int reset_cansee=IS_SET(ch->dyn, DYN_MOB_SEE_ALL);
	int reset_running_prog=IS_SET(ch->dyn,DYN_RUNNING_MOBPROG_CMD);
    program_flow( &mproglist_item, ch, vch, (void *)obj1, (void *)obj2 );
	if(reset_cansee){
		SET_BIT(ch->dyn, DYN_MOB_SEE_ALL);
	}
	if (reset_running_prog){
		SET_BIT(ch->dyn,DYN_RUNNING_MOBPROG_CMD);
	}
}
/**************************************************************************/
/*
 * Forces the mobile to flee.
 *
 * Syntax: mob flee
 *
 */
void do_mpflee( char_data *ch, char *)
{
    ROOM_INDEX_DATA *was_in;
    EXIT_DATA *pexit;
    int door, attempt;

    if ( ch->fighting != NULL )
	return;

    if ( (was_in = ch->in_room) == NULL )
	return;

    for ( attempt = 0; attempt < MAX_DIR; attempt++ )
    {
        door = number_door( );
        if ( ( pexit = was_in->exit[door] ) == 0
        ||   pexit->u1.to_room == NULL
        ||   IS_SET(pexit->exit_info, EX_CLOSED)
        || ( IS_NPC(ch)
        &&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
            continue;

        move_char( ch, door, false );
        if ( ch->in_room != was_in )
	    return;
    }
}
/**************************************************************************/
/*
 * Lets the mobile to transfer an object. The object must be in the same
 * room with the mobile.
 *
 * Syntax: mob otransfer [item name] [location]
 */
void do_mpotransfer( char_data *ch, char *argument )
{
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    char arg[ MIL ];
    char buf[ MIL ];
	
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
		mpbug( "MpOTransfer - Missing arg1 (item name).");
		return;
    }
    one_argument( argument, buf );
    if ( ( location = find_location( ch, buf ) ) == NULL )
    {
		mpbug( "MpOTransfer - arg2 location '%s' not found.", buf);
		return;
    }
    if ( (obj = get_obj_here( ch, arg )) == NULL )
		return;
    if ( obj->carried_by == NULL ){
		obj_from_room( obj );
    }else{
		if ( obj->wear_loc != WEAR_NONE )
			unequip_char( ch, obj );
		obj_from_char( obj );
    }
    obj_to_room( obj, location );
}
/**************************************************************************/
/*
 * Lets the mobile to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: mob remove <victim> <[#*]objectvnum|'all'>
 *   #* = the number of items with that vnum to remove... e.g. 5*350 will
 *        remove up to 5 of vnum 350 from victim.
 */
void do_mpremove( char_data *ch, char *argument )
{
    char_data *victim;
    OBJ_DATA *obj, *obj_next;
    vn_int vnum = 0;
    bool fAll = false;
    char arg[ MIL ];
	int number=20000;
	
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, arg ) ) == NULL ){
 		mpbug( "MpRemove: Couldnt find '%s' in the room to remove from.", arg);
		return;
	}
	
    one_argument( argument, arg );

	if(str_infix("*", arg)){ // support 2*<vnum> syntax
		number = mult_argument(argument,arg); 
	}
    if ( !str_cmp( arg, "all" ) ){
		fAll = true;
	}else if ( !is_number( arg ) ){
 		mpbug( "MpRemove: argument needs to be either the word 'all' or the vnum of an object, "
			"'%s' is neither of these... you can put <number>*<vnum> to remove up to the specific number worth of items.", arg);
		return;
    }else{
		vnum = atoi( arg );
	}


	int count=0;
    for ( obj = victim->carrying; obj; obj = obj_next )
    {
		obj_next = obj->next_content;
		if ( fAll || (obj->pIndexData->vnum == vnum && count<number) )
		{
			unequip_char( ch, obj );
			obj_from_char( obj );
			extract_obj( obj );
			count++;
		}
    }
}
/**************************************************************************/
/*
 * Mob inflict:
 * Identical in function to Mob Damage, however, it does not depend
 * on armor, it's raw damage.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob inflict <victim> <min> <max> [kill]
 */
void do_mpinflict( char_data *ch, char *argument )
{
	char_data *victim = NULL, *victim_next;
	char target[ MIL ],
	min[ MIL ],
	max[ MIL ],
	killoption[ MIL ];
	int low, high, dam;
	bool fAll = false, fKill = false;

	argument = one_argument( argument, target );
	argument = one_argument( argument, min );
	argument = one_argument( argument, max );

	if ( target[0] == '\0' )
	{
		mpbug("MpInflict - empty target field '%s'", target);
		return;
	}
	if( !str_cmp( target, "all" ) ){
		fAll = true;
	}else{
		if( ( victim = get_char_room( ch, target ) ) == NULL ){
			return;
		}
	}

	if ( is_number( min ) ){
		low = atoi( min );
	}else{
		mpbug("MpInflict - Bad damage min amount '%s' - must be numeric.", min);
		return;
	}

	if ( is_number( max ) ){
		high = atoi( max );
	}else{
		mpbug("MpInflict - Bad damage max amount '%s' - must be numeric.", max);
		return;
	}

	one_argument( argument, killoption);
	// If kill parameter is omitted, this command is "safe" and will not
	// kill the victim.
	if(!IS_NULLSTR(killoption)){
		if ( !str_cmp(killoption,"kill")){
			fKill = true;
		}else{
			mpbug("MpInflict - last optional parameter must be the word 'kill'.");
			return;
		}
	}

	dam = number_range(low, high);

	if ( fAll )
	{
		for( victim = ch->in_room->people; victim; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( victim != ch ) {
				// Does the checking to see how truly lethal the inflict will be
				// We don't want to take anyone below 5 hp if non-lethal
				if (!fKill){
					dam=UMIN(dam, victim->hit-5);
				}
				victim->hit -= dam;
				if ( !IS_NPC(victim)
					&& victim->level >= LEVEL_IMMORTAL
					&& victim->hit < 1 )
					victim->hit = 1;

				// kill them even if from in a prekill trigger
				if(fKill && victim->fighting==NULL && victim->hit<1){
    				damage( victim, victim, 200,
						TYPE_UNDEFINED, DAM_NONE, false );
				}

				update_pos (victim);
			}
		}
	}else{
		// Does the checking to see how truly lethal the inflict will be
		// We don't want to take anyone below 5 hp if non-lethal
		if (!fKill){
			dam=UMIN(dam, victim->hit-5);
		}

		victim->hit -= dam;
		if ( !IS_NPC(victim)
			&& victim->level >= LEVEL_IMMORTAL
			&& victim->hit < 1 )
			victim->hit = 1;

		// kill them even if from in a prekill trigger
		if(fKill && victim->fighting==NULL && victim->hit<1){
    		damage( victim, victim, 200,
				TYPE_UNDEFINED, DAM_NONE, false );
		}

		update_pos (victim);
	}
	return;
}
/**************************************************************************/
/*
 * Mob follow
 * Allows a mob to follow target
 * This is done silently, so echoes are needed if that is desired
 * Syntax: mob mpfollow [victim]
 */
void do_mpfollow( char_data *ch, char *argument )
{
	char		arg[MIL];
	char_data	*victim;

	if ( !IS_NPC( ch ))
	{
		do_huh( ch, "" );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
		return;				// blank

	if (( victim = get_char_room( ch, arg )) == NULL )
		return;				// no one at home

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master != NULL )
		return;				// charmed

	if ( victim == ch )
	{
		if ( ch->master == NULL )
		{
			return;			// already following self
		}
		SET_BIT(ch->dyn,DYN_SILENTLY);
		stop_follower( ch );
		REMOVE_BIT(ch->dyn,DYN_SILENTLY);
		return;				// stop following anyone
	}

	if ( ch->master != NULL )
		stop_follower( ch );
	SET_BIT(ch->dyn,DYN_SILENTLY);
	add_follower( ch, victim );
	REMOVE_BIT(ch->dyn,DYN_SILENTLY);
	return;
}

/**************************************************************************/
// Sets mob sneak on|off|debug
// This way the mob can sneak without having to be a thief mob
//
// Syntax: mob mpsneaky {=rn|off|debug}

void do_mpsneaky( char_data *ch, char *argument)
{
	char arg[MIL];
	char buf[MIL];

	if ( !IS_NPC( ch ))
	{
		do_huh( ch, "" );
		return;
	}

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
		return;				// blank

	if ( !str_cmp( arg, "on" ))
	{
		SET_BIT( ch->affected_by, AFF_SNEAK	);
	}

	if ( !str_cmp( arg, "off" ))
	{
		REMOVE_BIT( ch->affected_by, AFF_SNEAK );
	}

	if ( !str_cmp( arg, "debug" ))
	{
		sprintf( buf, "Sneaking is currently set to: `Y%s`x\r\n",
		IS_SET( ch->affected_by, AFF_SNEAK ) ? "ON" : "OFF" );
		act( buf, ch, NULL, NULL, TO_ROOM );		
	}
	return;
}
/**************************************************************************/
// Lags victim X * PULSE_VIOLENCE
//
// Syntax: mob stun $n {1-20}

void do_mpstun( char_data *ch, char *argument )
{
	char		arg[MIL];
	char		arg1[MIL];
	char_data	*victim;
	int			value;

	if ( !IS_NPC( ch ))
	{
		do_huh( ch, "" );
		return;
	}

	argument = one_argument( argument, arg  );
	argument = one_argument( argument, arg1 );
	
	value = atoi( arg1 );

	if ( value < 1 || value > 30 )
	{
		mpbug( "Mpstun: bad value %d - valid range is from 1 to 30", value );
		return;
	}

	if ( arg[0] == '\0' )
		return;				// blank

	if (( victim = get_char_room( ch, arg )) == NULL )
		return;				// no one at home

	WAIT_STATE( victim, value * PULSE_AFFECTS );
}
/**************************************************************************/
// Kal 
void do_mpletpass( char_data *, char *argument )
{
	extern bool mp_exit_trigger_no_letpass;
    if ( !IS_NULLSTR(argument)){
		mpbug("mob letpass - should have no arguments!");
		return;
	}

	mp_exit_trigger_no_letpass=false;
}

/**************************************************************************/
// Kal 
void do_mppreventattack( char_data *, char *argument )
{
	extern bool mp_prekill_trigger_result;
    if ( !IS_NULLSTR(argument)){
		mpbug("mob do_mppreventattack - should have no arguments!");
		return;
	}

	mp_prekill_trigger_result=true;
}
/**************************************************************************/
// Kal 
void do_mpseeall( char_data *mob, char *argument )
{
	if(!IS_NPC(mob)){
		mpbug("mob do_mpseeall - called by pc character!");
		return;	
	}
	
	SET_BIT(mob->dyn,DYN_MOB_SEE_ALL);
    if ( !IS_NULLSTR(argument)){
		mpbug("mob do_mpseeall - should have no arguments!");
		return;
	}
}
/**************************************************************************/
// Kal 
void do_mpnoseeall( char_data *mob, char *argument )
{
	if(!IS_NPC(mob)){
		mpbug("mob do_mpnoseeall - called by pc character!");
		return;	
	}
	
	REMOVE_BIT(mob->dyn,DYN_MOB_SEE_ALL);
    if ( !IS_NULLSTR(argument)){
		mpbug("mob do_mpnoseeall - should have no arguments!");
		return;
	}
}
/**************************************************************************/
void do_mpmsptochar( char_data *ch, char *argument )
{
	char name[MIL];
	char_data *victim;

	argument = one_argument(argument, name);

	if ( IS_NULLSTR(name)){
	    mpbug( "do_mpmsptochar: no name or sound specified to play.");
		return;
	}

	if ((victim=get_char_room( ch, name)) == NULL ){
	    mpbug( "do_mpmsptochar: couldn't find in room '%s' to play sound to.", name);
		return;
	}

	msp_to_room(MSPT_MOBPROG, argument, 
					0, 
					victim,
					true,
					false);
	ch->printlnf("msptochar: sent mobprog msp sound '%s' to %s.", argument, 
		PERS(victim, ch));
}
/**************************************************************************/
void do_mpmsptoroom( char_data *ch, char *argument )
{
	char sound[MIL];

	argument = one_argument(argument, sound);

	if ( IS_NULLSTR(sound)){
	    mpbug( "do_mpmsptoroom: no sound specified to play.");
		return;
	}

    for ( char_data	*vch = ch->in_room->people; vch; vch = vch->next_in_room)
	{
		msp_to_room(MSPT_MOBPROG, sound, 
						0, 
						vch,
						true,
						false);
		ch->printlnf("msptoroom: sent mobprog msp sound '%s' to %s.", sound, 
			PERS(vch, ch));
	}
	// send to the surrounding rooms
	msp_to_room(MSPT_MOBPROG, sound, 
					0, 
					ch,
					true, // 
					true);
	ch->printlnf("msptoroom: sent msp sound '%s' to surrounding rooms.", sound);
}
/**************************************************************************/
void do_dupeobj( char_data *ch, char *argument )
{
    char			arg[MIL];
    OBJ_DATA		*obj;
    OBJ_DATA		*obj_next;
	OBJ_INDEX_DATA	*dupeIndex;
	OBJ_DATA		*dupe;

    one_argument( argument, arg );
	
    if ( arg[0] == '\0')
		return;

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		if ( is_name( arg, obj->name ) )
		{
			dupeIndex = get_obj_index( obj->pIndexData->vnum );
			dupe = create_object( dupeIndex);
			obj_to_char( dupe, ch );
		}
	}	
	return;
}
/**************************************************************************/
// Kal 
void do_mppreventmove( char_data *, char *argument )
{
	extern bool mp_premove_trigger_allowmove;
	extern int active_premove_trigger_count;

    if ( !IS_NULLSTR(argument)){
		mpbug("mob preventmove - should have no arguments!");
		return;
	}

	if(active_premove_trigger_count<0){
		mpbug("mob preventmove: active_premove_trigger_count is %d - "
			"less than 0 - therefore no premove is currently running.", active_premove_trigger_count);
		return;
	}
	mp_premove_trigger_allowmove=false;
}

/**************************************************************************/
void do_mpwander( char_data *ch, char *)
{
	int door;
	for(int i=1; i<MAX_DIR;i++){
		door=number_range(0,MAX_DIR-1);
		if (ch->in_room->exit[door]  
			&& ch->in_room->exit[door]->u1.to_room   
			&& can_see_room(ch,ch->in_room->exit[door]->u1.to_room))
		{
			ch->print("You wander.");
			break;
		}
	}
    move_char( ch, door, false );
    return;
}

/**************************************************************************/
bool find_flexivalue(char_data *ch, char*argument, int *result, 
					 int base_value, int min_value, int max_value);

/**************************************************************************/
void do_mpsetskill( char_data *ch, char *argument )
{
	char_data	*victim;
	char		name[MIL];
	char		skill[MIL];
	char		arg0[MIL];	// to extract percentage
	char		arg1[MIL];	// to extract pracs
	char		arg2[MIL];	// to extract trains
	int			sn;
	int			percent= 0;
	int			pracs  = 0;
	int			trains = 0;

	argument = one_argument( argument, name  );
	argument = one_argument( argument, skill );
	argument = one_argument( argument, arg0  );
	argument = one_argument( argument, arg1  );
	argument = one_argument( argument, arg2  );

	pracs	= atoi( arg1 );
	trains  = atoi( arg2 );

	if ( IS_NULLSTR( name ))
		return;

	if (( victim = get_char_room( ch, name )) == NULL )
	{
		mpbug("mpsetskill: couldn't find victim '%s' in room %d",
				name, ch->in_room?ch->in_room->vnum:0);
		return;
	}

	if ( IS_NPC( victim ))
	{
		mpbug("mpsetskill: tried setting a skill on a MOB" );
		return;
	}

	if (( sn = skill_lookup( skill )) < 0 )
	{
		mpbug("mpsetskill: %s... no such skill/spell", skill );
		return;
	}
	
	if(!find_flexivalue(ch, arg0, &percent, victim->pcdata->learned[sn], 0, 100))
	{
		mpbug("mpsetskill: bad flexivalue input %s.", arg0);
		return;
	}


	if ( percent < 0 || percent > 100 )
	{
		mpbug( "mpsetskill: bad percentage value %d - valid range is from 0 to 100", percent );
		return;
	}

	if ( pracs < -50 || pracs > 100 )
	{
		mpbug( "mpsetskill: bad pracs value %d - valid range is from -50 to 100", pracs );
		return;
	}
	
	if ( trains < -10 || trains > 10 )
	{
		mpbug( "mpsetskill: bad trains value %d - valid range is from -10 to 10", trains );
		return;
	}

	if ( victim->practice - pracs < 0 )
	{
		mpbug( "mpsetskill: this would leave the player with -ve pracs, use haspracs ifcheck" );
		return;
	}

	if ( victim->train - trains < 0 )
	{
		mpbug( "mpsetskill: this would leave the player with -ve trains, use hastrains ifcheck" );
		return;
	}

	victim->pcdata->learned[sn] = percent;
	victim->practice			-= pracs;
	victim->train				-= trains;
}
/**************************************************************************/
void do_mpaffect( char_data *ch, char *argument )
{
	char_data	*victim;
	char		arg0[MIL];	// to extract percentage
	char		arg1[MIL];	// to extract pracs
	char		arg2[MIL];	// to extract trains
	int			value;

	argument = one_argument( argument, arg0  );
	argument = one_argument( argument, arg1  );
	argument = one_argument( argument, arg2  );

	if (( victim = get_char_room( ch, arg0 )) == NULL )
	{
		mpbug("mpaffect: couldn't find victim '%s' in room %d",
				arg0, ch->in_room?ch->in_room->vnum:0);
		return;
	}

	if ( IS_NPC( victim ))
	{
		mpbug("mpaffect: tried setting a value on a MOB, PC's only!!!" );
		return;
	}

	value = atoi( arg2 );

	if ( !str_cmp( arg1, "tired" ))
	{
		victim->pcdata->tired += value;
	}
	if ( !str_cmp( arg1, "hunger" ))
	{
		gain_condition( victim, COND_HUNGER, -value );
	}
	if ( !str_cmp( arg1, "thirst" ))
	{
		gain_condition( victim, COND_THIRST, -value );
	}
	return;
}
/**************************************************************************/
void do_mpzuecho( char_data *ch, char *argument )
{
    connection_data *d;
    
    if ( argument[0] == '\0' )
    {
		mpbug( "MpZUEcho: missing argument from vnum %d",
			IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
    }

	if ( ch->in_room == NULL )
		return;

    for ( d = connection_list; d; d = d->next )
    {
		if ( d->connected_state == CON_PLAYING )
		{
			if ( d->character->in_room->sector_type == SECT_UNDERWATER
			&&   IS_AWAKE(d->character)
			&&	 d->character->in_room != NULL
			&&   ch->in_room != NULL
			&&   d->character->in_room->area == ch->in_room->area )
			{
				if ( IS_IMMORTAL(d->character))
					d->character->print( "zoneunderwater> ");
				d->character->printf("%s`x\r\n", argument );
			}
		}
	}

    return;
}

/**************************************************************************/
// Kal 
void do_mploginprocessed( char_data *, char *argument )
{
	extern bool mp_login_trigger_processed;

    if ( !IS_NULLSTR(argument)){
		mpbug("mob loginprocessed - should have no arguments!");
		return;
	}
	mp_login_trigger_processed=true;
}
/**************************************************************************/
// Kal 
void do_mplogoutprocessed( char_data *, char *argument )
{
	extern bool mp_logout_trigger_processed;

    if ( !IS_NULLSTR(argument)){
		mpbug("mob logoutprocessed - should have no arguments!");
		return;
	}
	mp_logout_trigger_processed=true;
}
/**************************************************************************/
// Kal - April 00 :)
void do_mpinroom( char_data *ch, char *argument )
{
	if(IS_NULLSTR(argument)){
		mpbug("do_mpinroom() no command specified"
			"Syntax: mob inroom <command>\n"
			"Notes: inroom removes you ability see objects and mobs outside the room\n"
			"       you are currently in, this is useful to specifically find an object\n"
			"       with stat, or set, and stat into a mob in the current room etc.\n");
		return;
	}

	SET_BIT(ch->dyn,DYN_IN_ROOM_ONLY);
	ch->println("MOB INROOM BEGINS");
	// do the command
	interpret( ch, argument );
	ch->println("MOB INROOM ENDS");
	REMOVE_BIT(ch->dyn,DYN_IN_ROOM_ONLY);

}

/**************************************************************************/
// mob swipe $n [gold|silver] %
void do_mpswipe( char_data *ch, char *argument )
{
	char_data	*victim;
	char		arg0[MIL];	// victim
	char		arg1[MIL];	// holds [gold|silver]
	char		arg2[MIL];	// to extract the percentage value
	int			percent, value;
	bool		foundcurrency = false;

	if (IS_NULLSTR(argument)){
		mpbug( "do_mpswipe(): null argument" );
		return;
	}

	argument = one_argument( argument, arg0  );
	argument = one_argument( argument, arg1  );
	argument = one_argument( argument, arg2  );

	if (( victim = get_char_room( ch, arg0 )) == NULL )
	{
		mpbug("mpswipe(): couldn't find victim '%s' in room %d",
				arg0, ch->in_room?ch->in_room->vnum:0);
		return;
	}

	if ( !is_number( arg2 ))
	{
		mpbug("mpswipe(): non-numerical value used as a percentage value");
		return;
	}

	value = atoi( arg2 );
	percent = 100 - value;

	if ( percent < 1 || percent > 100 )
	{
		mpbug("mpswipe(): the percentage value must range between 1-100");
		return;
	}

	if ( !str_cmp( arg1, "gold" ))
	{
		foundcurrency = true;
		victim->gold = (victim->gold * percent)/100;
	}
	if ( !str_cmp( arg1, "silver" ))
	{
		foundcurrency = true;
		victim->silver = (victim->silver * percent)/100;	
	}

	if ( !foundcurrency )
	{
		mpbug("mpswipe(): the currency must be defined as either `Ygold`x or `ssilver`x." );
	}
	return;
}

/**************************************************************************/
// Kal, Aug 2002
void do_mpxpreward( char_data *ch, char *argument )
{
	char_data	*victim;
	char		name[MIL];
	char		amount[MIL];
	int value;

	argument = one_argument( argument, name  );
	argument = one_argument( argument, amount );

	if(IS_NULLSTR( amount)){
		mpbug("mpxpreward: incomplete command.");
		return;
	}

	if (( victim = get_char_room( ch, name )) == NULL ){
		mpbug("mpxpreward: couldn't find victim '%s' in room %d",
				name, ch->in_room?ch->in_room->vnum:0);
		return;
	}

	if ( IS_NPC( victim )){
		mpbug("mpxpreward: tried setting xp on a MOB" );
		return;
	}

	if(!is_number(amount)){
		mpbug("mpxpreward: amount '%s' is not a numeric value.", amount );
		return;
	}
	value=atoi(amount);

	if ( value< -1000 || value> 1000 || value==0){
		mpbug("mpxpreward: amount '%d' is above 1000, below -1000 or zero.", value);
		return;
	}

	logf("mpxpreward: mprg%d mvnum%d room%d line%d call_level%d awarding %d xp to %s, xp before=%d.", 
		callstack_pvnum[call_level-1],
		callstack_mvnum[call_level-1],
		callstack_rvnum[call_level-1],
		callstack_line[call_level-1],
		call_level-1,
		value,
		victim->name,
		victim->exp);

	if(value>0){
		gain_exp(victim, value);
	}else{
		// apply the negative xp modifications
		ch->exp+=value;

		// can't get below level 1
		ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp);

		// check, shouldn't be required
		if(ch->pcdata->sublevel<0){
			ch->pcdata->sublevel=0;
		}
	}	
	save_char_obj(ch);
}
/**************************************************************************/
// Kal, Sept 02
void do_mppreventtrain( char_data *, char *argument )
{
    if ( !IS_NULLSTR(argument)){
		mpbug("mob preventtrain - should have no arguments!");
		return;
	}

	mobprog_preventtrain_used=true;
}
/**************************************************************************/
// Kal, Sept 02
void do_mppreventprac( char_data *, char *argument )
{
    if ( !IS_NULLSTR(argument)){
		mpbug("mob preventprac - should have no arguments!");
		return;
	}

	mobprog_preventprac_used=true;
}

/**************************************************************************/
/**************************************************************************/
