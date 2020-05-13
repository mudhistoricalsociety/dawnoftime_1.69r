/**************************************************************************/
// score.cpp - The score table
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
#include "include.h"
#include "ictime.h"
#include "cust_col.h"

/**************************************************************************/
extern char randomColours[16];
int get_sublevels_for_level(int level);
void do_affects(char_data *ch, char * argument);

/**************************************************************************/
#define ch_custom_colour(cc_code) (ch->desc->colour_memory.custom_colour[(cc_code)])
#define ch_template_colour(cc_code) (ch->desc->colour_memory.colour_template->template_colour[(cc_code)])
#define ch_effective_colour(cc_code) (ch_custom_colour(cc_code)=='.'? \
	ch_template_colour(cc_code):ch_custom_colour(cc_code))

/**************************************************************************/
//By Kalahn - Based on original oblivion score table
void show_score( char_data *ch, char_data *v)
{
	// figure out if remort is shown
	char rbuf[MIL];
	if(!IS_NPC(v) 
		&& GAMESETTING(GAMESET_REMORT_SUPPORTED) 
		&& GAMESETTING(GAMESET_REMORT_IN_SCORE))		
	{
		sprintf(rbuf,"Remort: `G%d", v->remort);
	}else{
		strcpy(rbuf,"`x");
	}

	// minor little hack to allow random custom border colours on scores to
	// appear as a single or pair of colours 
	bool restore_random_border=false;
	bool restore_random_inner_border=false;
	if(ch->desc){
		if(ch_effective_colour(CC_SCORE_BORDER)=='?'){
			ch_effective_colour(CC_SCORE_BORDER)=randomColours[number_range(0, 14)];
			restore_random_border=true;
		}

		if(ch_effective_colour(CC_SCORE_INNER_BORDER)=='?'){
			ch_effective_colour(CC_SCORE_INNER_BORDER)=randomColours[number_range(0, 14)];
			restore_random_inner_border=true;
		}				
	}
	if(restore_random_border && restore_random_inner_border){
		ch_effective_colour(CC_SCORE_INNER_BORDER)=	UPPER(ch_effective_colour(CC_SCORE_BORDER));
		ch_effective_colour(CC_SCORE_BORDER)=LOWER(ch_effective_colour(CC_SCORE_BORDER));
	}

	// determine what to show for the players name 
	char fullname[MSL];
	if(v->pcdata && !IS_NULLSTR(v->pcdata->surname)){
		sprintf(fullname, "%s %s", capitalize(v->name), capitalize(v->pcdata->surname));
	}else{
		strcpy(fullname, capitalize(v->name));
	}

	// determine what letgain text to show
	const char *letgained_text="";
	if(!GAMESETTING(GAMESET_NO_LETGAINING_REQUIRED)){
		letgained_text=IS_LETGAINED(v)?"  (letgained)  ":"(not letgained)";
	}

	// first section
	ch->println("`=S_______________________________________________________________________________");  
	ch->println("`=S|\\`=s___________________________________________________________________________`=S/|");
	ch->printlnf("`=S|`=s|`x  Name:  `=g%-36s `=G%-16s `x%-12s  `=s|`=S|", 
		fullname, letgained_text, rbuf);


	char leveltxt[MIL*2];
	if(IS_ICIMMORTAL(v) && GAMESETTING4(GAMESET4_SHOW_IMMROLE_IN_SCORE_INSTEAD_OF_LEVEL)){
		if(IS_NULLSTR(v->pcdata->imm_role)){
			sprintf(leveltxt, "Role: `R%-15s", " undefined");
			// display note teaching high admin how to set a players role text
			if(IS_TRUSTED(ch,MAX_LEVEL-1)){
				ch->println("Note: the role text below is set using 'set char <immname|self> immrole <text to appear>");
			}
		}else{
			sprintf(leveltxt, "Role: `W%s", c_str_len(v->pcdata->imm_role)>14?str_width(v->pcdata->imm_role,15):
				FORMATF(" %s",str_width(v->pcdata->imm_role,14)));
		}
	}else{
		sprintf(leveltxt, "Level: `R%3d           ", v->level);
	}

	ch->printlnf("`=S|`=s|`x  %s`xRace: `B%-11s`xGender: `B%-7s `xClass: `B%-12s`=s|`=S|",
	  leveltxt,
	  race_table[v->race]->name,
	  v->sex==0 ? "sexless" : v->sex==1 ? "male" : "female",
	  class_table[v->clss].name);
	
	if(!IS_NPC(v)){
		if(v->pcdata->birthdate){
			ch->printlnf("`=S|`=s|`x  Born:  `Y%s `x  Age: `y%-44s   `=s|`=S|",
				str_width( get_shorticdate_from_time(v->pcdata->birthdate, 
							"`#%d`S/`^%d`S/`&%d", v->pcdata->birthyear_modifier),11),
				get_ictimediff(v->pcdata->birthdate, current_time, -v->pcdata->birthyear_modifier));
			
		}else{
			ch->printlnf("`=S|`=s|`x  Born:  `Y%s `x  Age: `y%-44s   `=s|`=S|",
				str_width(get_shorticdate_from_time(
					v->id - (17*ICTIME_IRLSECS_PER_YEAR), 
						"`#%d`S/`^%d`S/`&%d", 0),11),
			get_ictimediff(v->id-(17*ICTIME_IRLSECS_PER_YEAR), 
				current_time, 0));
		}	
	}

	if(v->practice>9999){
		ch->printlnf("`=S|`=s|`x  Practices:`r%5d `xExperience: `r%-12d`x Role Playing Score: `r%7ld     `=s|`=S|",
		  v->practice,
		  v->exp,
		  IS_NPC(v) ? 0 : v->pcdata->rp_points);
	}else{
		ch->printlnf("`=S|`=s|`x  Practices:`r%4d  `xExperience: `r%-12d`x Role Playing Score: `r%7ld     `=s|`=S|",
		  v->practice,
		  v->exp,
		  IS_NPC(v) ? 0 : v->pcdata->rp_points);
	}

	// format the karns text
	char karns_text[MIL];
	if(GAMESETTING4(GAMESET4_NO_KARN_IN_SCORE)){
		strcpy(karns_text,"          ");
	}else{
		sprintf(karns_text, "Karns: `R%d`S/`r%d", IS_NPC(v) ? 0 : v->pcdata->karns, GET_MAX_KARN(v));
	}
		   
	if(v->train>9999){
		ch->printlnf("`=S|`=s|`x  Trains:  `r%6d `xXP Till Next Level: `R%-7d         `x  %s         `=s|`=S|",
		  v->train,
		  IS_NPC(v) ? 0 : (v->level+1)*exp_per_level(v,v->pcdata->points)-(v->exp),
		  karns_text);
	}else{
		if(GAMESETTING3(GAMESET3_SHOW_QP_IN_SCORE)){
			ch->printlnf("`=S|`=s|`x  Trains:   `r%4d  `xXP Till Next Level: `R%-7d        `xQP:    `B%-3d`x %s `=s|`=S|",
			  v->train,
			  IS_NPC(v) ? 0 : (v->level+1)*exp_per_level(v,v->pcdata->points)-(v->exp),
			  IS_NPC(v) ? 0 : v->pcdata->qpoints,
			  karns_text);
		}else{
			ch->printlnf("`=S|`=s|`x  Trains:   `r%4d  `xXP Till Next Level: `R%-7d         `x  %s         `=s|`=S|",
			  v->train,
			  IS_NPC(v) ? 0 : (v->level+1)*exp_per_level(v,v->pcdata->points)-(v->exp),
			  karns_text);
		}
	}

	if(!IS_NPC(v) && get_sublevels_for_level(v->level)){
		ch->printlnf("`=S|`=s|`x  Sublevel:  %2d/%2d  Subprac: %2d  Subtrain: %2d                              `=s|`=S|", 
			v->pcdata->sublevel, get_sublevels_for_level(v->level),
			v->pcdata->sublevel_pracs, v->pcdata->sublevel_trains);
	}

	ch->println("`=S|`=s|___________________________________________________________________________|`=S|");
	ch->println("`=S|/___________________________________________________________________________\\|");
	ch->println("`=S|\\`=s________________________________`=S/| |\\`=s______________________________________`=S/|");
	// attribute and info section
	ch->printlnf("`=S|`=s|`xSt:%3d(%3d)/%+3d%sCo:%3d(%3d)/%+3d%s`=s|`=S| "
				 "|`=s| `xHit Points: `R%5d`x/`r%5d  `xAlliance: `B%-2d`=s|`=S|",
		v->perm_stats[STAT_ST], v->potential_stats[STAT_ST], 
			v->modifiers[STAT_ST], abs(v->modifiers[STAT_ST])>99?"":" ",
		v->perm_stats[STAT_CO], v->potential_stats[STAT_CO], 
			v->modifiers[STAT_CO],abs(v->modifiers[STAT_CO])>99?"":" ",
		v->hit,  v->max_hit,  IS_NPC(v) ? 0 : v->alliance);

	ch->printlnf("`=S|`=s|`xQu:%3d(%3d)/%+3d%sAg:%3d(%3d)/%+3d%s`=s|`=S| "
				 "|`=s| `xMana:       `R%5d`x/`r%5d  `xTendency: `B%-2d`=s|`=S|",
		v->perm_stats[STAT_QU], v->potential_stats[STAT_QU], 
			v->modifiers[STAT_QU],abs(v->modifiers[STAT_QU])>99?"":" ",
		v->perm_stats[STAT_AG], v->potential_stats[STAT_AG], 
			v->modifiers[STAT_AG],abs(v->modifiers[STAT_AG])>99?"":" ",
		v->mana, v->max_mana,  IS_NPC(v) ? 0 : v->tendency);

	ch->printlnf("`=S|`=s|`xPr:%3d(%3d)/%+3d%sSd:%3d(%3d)/%+3d%s`=s|`=S| "
				 "|`=s| `xHitroll:  `R%1s %3d `xDamroll: `R%1s %3d       `=s|`=S|",
		v->perm_stats[STAT_PR], v->potential_stats[STAT_PR], 
			v->modifiers[STAT_PR],abs(v->modifiers[STAT_PR])>99?"":" ",
		v->perm_stats[STAT_SD], v->potential_stats[STAT_SD], 
			v->modifiers[STAT_SD],abs(v->modifiers[STAT_SD])>99?"":" ",
		(GET_HITROLL(v)<0) ? "-" : (GET_HITROLL(v)==0) ? " " : "+", abs(GET_HITROLL(v)),
		(GET_DAMROLL(v)<0) ? "-" : (GET_DAMROLL(v)==0) ? " " : "+", abs(GET_DAMROLL(v)));


	ch->printlnf("`=S|`=s|`xEm:%3d(%3d)/%+3d%sMe:%3d(%3d)/%+3d%s`=s|`=S| "
		"|`=s| `xACpierce: `B%1s%3d  `xACbash:  `B%1s%3d        `=s|`=S|",
		v->perm_stats[STAT_EM], v->potential_stats[STAT_EM], 
			v->modifiers[STAT_EM],abs(v->modifiers[STAT_EM])>99?"":" ",
		v->perm_stats[STAT_ME], v->potential_stats[STAT_ME], 
			v->modifiers[STAT_ME],abs(v->modifiers[STAT_ME])>99?"":" ",
		(GET_AC(v,AC_PIERCE)/20<0) ? "-": (GET_AC(v,AC_PIERCE)/20)==0 ? " " : "+", 
		abs(GET_AC(v,AC_PIERCE)/20),(GET_AC(v,AC_BASH)/20<0) ? "-": 
		(GET_AC(v,AC_BASH)/20)==0 ? " " : "+", abs(GET_AC(v,AC_BASH)/20));

	ch->printlnf("`=S|`=s|`xIn:%3d(%3d)/%+3d%sRe:%3d(%3d)/%+3d%s`=s|`=S| "
				   "|`=s| `xACslash:  `B%1s%3d  `xACmagic: `B%1s%3d        `=s|`=S|",
		v->perm_stats[STAT_IN], v->potential_stats[STAT_IN], 
			v->modifiers[STAT_IN],abs(v->modifiers[STAT_IN])>99?"":" ",
		v->perm_stats[STAT_RE], v->potential_stats[STAT_RE], 
			v->modifiers[STAT_RE],abs(v->modifiers[STAT_RE])>99?"":" ",
	  (GET_AC(v,AC_SLASH)/20<0) ? "-" : (GET_AC(v,AC_SLASH)/20)==0 ? " " : "+",
	  abs(GET_AC(v,AC_SLASH)/20), (GET_AC(v,AC_EXOTIC)/20<0) ? "-" : 
	  (GET_AC(v,AC_EXOTIC)/20)==0 ? " " : "+",   abs(GET_AC(v,AC_EXOTIC)/20));

	ch->println("`=S|`=s|________________________________|`=S| |`=s|______________________________________|`=S|");
	ch->println("`=S|/________________________________\\| |/______________________________________\\|");
	ch->println("`=S|\\`=s___________________________________________________________________________`=S/|");
	ch->printlnf("`=S|`=s| `YGold:%6ld  `sSilver:%6ld `xItems:%3d/%4d Encumbrance:%7d/%7d     `=s|`=S|",
		v->gold, v->silver, v->carry_number/10, can_carry_n(v)/10, 
		(int) get_carry_weight(v)/10, (int) can_carry_w(v)/10 );
	ch->println("`=S|`=s|___________________________________________________________________________|`=S|");
	ch->println("`=S|/___________________________________________________________________________\\|");

	if(IS_IMMORTAL(v))
	{
		ch->println("`=S|\\`=s___________________________________________________________________________`=S/|");
		ch->printlnf("`=S|`=s|`x  %10s%3d   %5s%3d   %5s%3d   %5s%3d  Trust:%3d         `=s|`=S|",
			"Wizi:",	v->invis_level, 
			"OWizi:",	v->owizi, 
			"IWizi:",	v->iwizi, 
			"Incognito:", v->incog_level, v->trust);
		ch->println("`=S|`=s|___________________________________________________________________________|`=S|");
		ch->println("`=S|/___________________________________________________________________________\\|`x");
	}

	if(restore_random_border){
		ch_effective_colour(CC_SCORE_BORDER)='?';
	}
	if(restore_random_inner_border){
		ch_effective_colour(CC_SCORE_INNER_BORDER)='?';
	}
	
	if (IS_SET(ch->comm,COMM_SHOW_AFFECTS)){
		if(v==ch){
			do_affects(ch,"");
		}else{ // imm version - checking out another player
			do_affects(ch,v->name);
		}
	}
}

/**************************************************************************/
void do_score( char_data *ch, char *argument )
{
    char arg[MIL];
    char_data *victim;

    one_argument( argument, arg );

    if (IS_IMMORTAL(ch) && !IS_NULLSTR(arg))
    {
		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			ch->println("They aren't here.");
			return;
		}
    }
	else
	{
		victim=ch;
	}
   
	show_score(ch, victim);
}
/**************************************************************************/
/**************************************************************************/

