/**************************************************************************/
// pload.cpp - dawn player loading/unloading system
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "pload.h"

void extract_char_from_char_list(char_data *ch);
int ploaded_players_in_realm=0;
/**************************************************************************/
char_data *pload_find_player_by_name(char *name)
{
	// the following lines are an optimisation so everytime a player
	// logs in we don't have to search every player in every room
	if(ploaded_players_in_realm<=0){
		return NULL;
	}

	// loop thru all players in all rooms looking for ploaded players
	int i;
	ROOM_INDEX_DATA *pRoomIndex;

	for(i=0; i<MAX_KEY_HASH; i++){
		for ( pRoomIndex  = room_index_hash[i];pRoomIndex;pRoomIndex = pRoomIndex->next ){
			for(char_data *lch=pRoomIndex->people; lch; lch=lch->next_in_room){
				if(lch->pload && !IS_NPC(lch)){
					if(!str_cmp(name, lch->name)){
						return lch;
					}
				}
			}
		}
	}
	// unfound
	return NULL;
}
/**************************************************************************/
// extract a ploaded player, without saving them
void pload_extract(char_data *ch, char_data *victim)
{
	if(!victim){
		ch->println("pload_extract(): bug, victim is NULL!");
		logf("pload_extract(): bug, victim is NULL!");
		return;
	}

	if(!victim->pload){
		ch->println("pload_extract(): victim->pload is NULL!");
		logf("pload_extract(): victim->pload is NULL!");
		return;
	}

	char_from_room(victim);	
	if(victim->pet){
		char_from_room(victim->pet);
	}
	
	if(victim->pload->loaded_by && victim->pload->loaded_by!=ch){
		victim->pload->loaded_by->printlnf("`YPloaded character '%s' has been unloaded by '%s'`x",
			PERS(victim, victim->pload->loaded_by),
			ch?PERS(ch, victim->pload->loaded_by):"player login?!?");
	}

	ch->printlnf("Ploaded character '%s' has been unloaded.", PERS(victim, ch));

	// deallocate the pload relationship recording
	if(victim->pload->loaded_by){
		victim->pload->loaded_by->ploaded=NULL;
		victim->pload->loaded_by=NULL;
		delete victim->pload;
		victim->pload=NULL;
	}
	if(victim->pet){
		if(victim->pet->pload){
			delete victim->pet->pload;
			victim->pet->pload=NULL;
		}
		free_char( victim->pet );
	}
	free_char( victim ); 

	ploaded_players_in_realm--;
}
/**************************************************************************/
void do_goto( char_data *ch, char *argument );
/**************************************************************************/
// Kal - Dec 2001
void do_pload(char_data *ch, char *argument )
{	
	char_data *lch;

	if(IS_NPC(ch) || ch->pload){
		ch->println("live players only sorry.");
		return;
	}

	char arg[MIL];
	char name[MIL];
	argument=one_argument(argument, arg);
	one_argument(argument, name);
	
	if(IS_NULLSTR(arg)){
		ch->titlebar("PLAYER LOAD SYNTAX AND NOTES");
		ch->println("syntax: pload load <playername>   - load a player");
		ch->println("syntax: pload unload <playername> - removes a ploaded player");
		ch->println("syntax: pload save <playername>   - saves a ploaded player pfile");
		ch->println("syntax: pload list <playername>   - lists all currently ploaded players");

		ch->println("");
		ch->println("`xNotes about a ploaded player:");
		ch->println("* Any changes you do to the player will NOT be saved,\r\n"
					"  unless you use the `=Cpload save`x command");
		ch->println("* The 'unload' option does NOT save the ploaded players pfile.");
		ch->println("* Ploaded players can not be seen by mortals");
		ch->println("* Time does NOT pass for the player, so affect durations will be unaffected");
		ch->println("* The player will automatically be unloaded (without saving) when you logout.");
		ch->println("* The player will automatically be unloaded (without saving)\r\n"
					"  if they try to login and get their password correct.");
		ch->println("* Ploaded players are put in limbo by default, if you move them from this\r\n"
					"  room and 'pload save' them, they will appear in the new room next login");
		ch->println("* A players ploaded login is not recorded in laston.");
		ch->println("* A players login time, site and hours played remain what\r\n"
					"  they were with their last normal login.");
		ch->println("* Ploaded players are not transfered nor saved for a hotreboot.");
		ch->println("* Often the ploaded player can only be located while in the same\r\n"
					"  room as it, or using the 'pload list' command.");
		ch->println("* With some functions, ploaded player names must be used in full.");
		ch->titlebar("");
		return;
	}

	// load option
	if(!str_cmp(arg,"load")){
		connection_data d;
		memset(&d, 0, sizeof(d));
		d.make_connected_socket_invalid();
		bool pfile_loaded_okay;

		if(IS_NULLSTR(name)){
			do_pload(ch,""); // show the help - incomplete syntax
			return;
		}

		ROOM_INDEX_DATA *limbo=get_room_index( ROOM_VNUM_LIMBO );
		assertp(limbo); // should be impossible to not have a limbo room

		// imms must be in the limbo room to pload players
		if(ch->in_room!=limbo){
			ch->printlnf("You must be in limbo (room %d) to load players with pload.",
				ROOM_VNUM_LIMBO);
			ch->println("Moving you to limbo for the pload.");
			interpret(ch, FORMATF("goto %d",ROOM_VNUM_LIMBO));
			if(ch->in_room!=limbo){
				ch->printlnf("The command 'goto %d' failed to get you to limbo, manually goto limbo then retry pload.",
					ROOM_VNUM_LIMBO);
				return;
			}
		}

		// can't pload more than one at a time.
		if(ch->ploaded){
			ch->printlnf("You have already ploaded '%s' who is in room %d.",
				ch->ploaded->name, ch->ploaded->in_room->vnum);
			return;
		}

		// confirm no player is in the game with exactly that name
		for( lch = player_list; lch; lch = lch->next_player ){
			if(!str_cmp(lch->name, name)){
				if(can_see_who(ch, lch)){
					ch->printlnf("'%s' is already within the game.", name);
				}else{
					ch->printlnf("'%s' is above your level/trust - you can not pload him/her.", name);
				}
				return;
			}
		}

		// confirm that character isn't ploaded by someone else
		if(pload_find_player_by_name(name)){
			ch->printlnf("'%s' is already ploaded by someone else.", name);
			do_pload(ch,"list");
			return;
		}
		
		// attempt to load the pfile
		pfile_loaded_okay= load_char_obj( &d, name );

		if(!pfile_loaded_okay || !d.character){
			ch->printlnf("Couldn't load pfile '%s'.", name);
			return;
		}

		// pfile loaded, check if they are above the trust/level of the loader
		lch=d.character;
		d.character->desc=NULL;
		if(lch->pet){
			// remove pets from the char_list 
			extract_char_from_char_list(lch->pet);			
		}

		if(get_trust(ch)<=get_trust(lch)){
			// notice that this message should be the same as the one above, if the loader
			// cant see the loaded person when they are already in the game
			ch->printlnf("'%s' is above your level/trust - you can not pload him/her.", name);

			// deallocate the loaded character data
			if(lch->pet){
				free_char( lch->pet );
			}
			free_char( lch );
			return;
		}

		// === By This Stage:
		// * we have loaded the pfile data into lch
		// * lch->in_room is set to the room the player would normally be loaded into
		// * if they have a pet:
		//   - pet is read into lch->pet, 
		//   - lch->pet->in_room is set to the room the pet would normally be loaded into
		
		// Transfer the player into the limbo room, save the was_in_room field first though
		// as this simulates the affect of the player going idle and being transfered to
		// limbo naturally... as a result when the player is saved, they will be saved in their 
		// original room.
		
		lch->was_in_room=lch->in_room;
		char_to_room(lch, limbo);

		ch->printlnf("Character %s ploaded - original room %d.", 
			PERS(lch, ch), lch->was_in_room?lch->was_in_room->vnum:-1);
		logf("Character '%s' ploaded - will save in original room (%d).", 
			PERS(lch, NULL), lch->was_in_room?lch->was_in_room->vnum:-1);

		if(lch->pet){
			lch->pet->was_in_room=lch->pet->in_room;
			char_to_room(lch->pet, limbo);
			ch->printlnf("Pet '%s' belonging to %s ploaded - original room %d.", 
				PERS(lch->pet, ch), PERS(lch, ch), lch->pet->was_in_room?lch->pet->was_in_room->vnum:-1);
			logf("Pet '%s' belonging to %s ploaded - original room vnum %d.", 
				PERS(lch->pet, NULL), PERS(lch, NULL), lch->pet->was_in_room?lch->pet->was_in_room->vnum:-1);
		}

		// record the pload relationship
		ch->ploaded=lch; 
		lch->pload=new pload_data;
		lch->pload->dont_save=true; // we dont want the pfile saved
		lch->pload->loaded_by=ch;
		lch->next=NULL;
		lch->next_player=NULL;
		ploaded_players_in_realm++;
		if(lch->pet){
			lch->pet->pload=new pload_data;
			lch->pet->pload->loaded_by=NULL;
			lch->pet->next=NULL;
			lch->pet->next_player=NULL;
		}
		
		// NOTE: neither pets nor characters are within the player_list or character_list
		//       This is intentional - so they arent affected by char_update() etc
		return;
	}


	// unload option
	if(!str_cmp(arg,"unload")){
		if(IS_NULLSTR(name)){
			do_pload(ch,""); // show the help - incomplete syntax
			return;
		}

		lch=get_char_room(ch, name);

		if(!lch){
			ch->printlnf("There is no ploaded player '%s' to unload from this room.", name);
			return;
		}

		if(IS_NPC(lch)){
			ch->printlnf("'%s' is an NPC, you can't use pload to unload them.", name);
			return;
		}

		if(!lch->pload){
			ch->printlnf("'%s' was not ploaded, so you can't unload him/her.", name);
			return;
		}

		pload_extract(ch, lch);
		return;
	}

	// save option
	if(!str_cmp(arg,"save")){
		if(IS_NULLSTR(name)){
			do_pload(ch,""); // show the help - incomplete syntax
			return;
		}

		lch=get_char_room(ch, name);

		if(!lch){
			ch->wraplnf("There is no ploaded player '%s' to save in this room "
				"(you must be in the same room as a ploaded player to save them).", name);
			do_pload(ch,"list");
			return;
		}

		if(IS_NPC(lch)){
			ch->printlnf("'%s' is an NPC, you can't use pload to save them.", name);
			return;
		}

		if(!lch->pload){
			ch->printlnf("'%s' was not ploaded, so you can't save him/her.", name);
			return;
		}

		lch->pload->dont_save=false; // we dont want the pfile saved
		save_char_obj(lch);
		lch->pload->dont_save=true; // we dont want the pfile saved
		ch->printlnf("saved ploaded player '%s'", lch->name);
		logf("saved ploaded player '%s'", lch->name);
		return;
	}

	// list option
	if(!str_cmp(arg,"list")){
		ROOM_INDEX_DATA *limbo=get_room_index( ROOM_VNUM_LIMBO );
		assertp(limbo); // should be impossible to not have a limbo room
		// loop thru all players in all rooms looking for ploaded players
		int i;
		int count=0;
		int pccount=0;
		ROOM_INDEX_DATA *pRoomIndex;

		for(i=0; i<MAX_KEY_HASH; i++){
			for ( pRoomIndex  = room_index_hash[i];pRoomIndex;pRoomIndex = pRoomIndex->next ){
				for(lch=pRoomIndex->people; lch; lch=lch->next_in_room){
					if(lch->pload){
						ch->printf("[%d] %s in room %d ", 
							++count,
							PERS(lch, ch),
							pRoomIndex->vnum);

						if(lch->in_room==limbo && lch->was_in_room){
							ch->printf("[save room %d] ", lch->was_in_room->vnum);
						}else{
							ch->printf("[save room %d] ", lch->in_room->vnum);
						}

						if(lch->pload->loaded_by){
							ch->printlnf("(loaded by %s)", PERS(lch->pload->loaded_by, ch));
						}else{
							if(IS_NPC(lch) && lch->master && lch->master->pet==lch){
								ch->printlnf("(pet of %s)", PERS(lch->master, ch));
							}else{
								ch->println("(loaded by unknown)");
							}
						}
						if(!IS_NPC(lch)){
							pccount++;
						}
					}
				}
			}
		}
		if(count==0){
			ch->println("There are no ploaded players in the game");
		}
		if(pccount!=ploaded_players_in_realm){
			logf("do_pload(): bug - count!=ploaded_players_in_realm!!!");
			ch->println("do_pload(): bug - count!=ploaded_players_in_realm!!!");
		}
		return;
	}

	ch->printlnf("Unknown pload command '%s'", arg);
	do_pload(ch, "");

}
/**************************************************************************/
// Kal, Aug 02
void do_undeny( char_data *ch, char *argument )
{
	if(IS_NULLSTR(argument)){
		ch->println("syntax: undeny <playername>");
		ch->println("This command is really a macro for:`1"
			" pload load <playername>`1"
			" undeny <playername>`1"
			" pload save <playername>`1"
			" pload unload <playername>");
		return;
	}
	char_data *victim;

	// check the player isn't already around
	victim=get_whovis_player_world(ch, argument);
	if(victim){
		ch->printlnf("player '%s' is already in the game.", argument);
		return;
	}
	victim=pload_find_player_by_name(argument);
	if(victim){
		ch->printlnf("ploaded player by name '%s' already in the game.", argument);
		return;
	}

	// backup the room they are already in
	room_index_data *rm=ch->in_room;
	ROOM_INDEX_DATA *limbo=get_room_index( ROOM_VNUM_LIMBO );
	assertp(limbo); // should be impossible to not have a limbo room

	ch->println("UNDENY BEGINS");
	// imms must be in the limbo room to pload players
	if(rm!=limbo){
		ch->println("Moving you to limbo for the pload.");
		char_from_room(ch);
		char_to_room(ch, limbo);
	}


	
	ch->println("Ploading victim.");
	interpret(ch, FORMATF("pload load %s", argument));

	victim=pload_find_player_by_name(argument);
	if(!victim || IS_NPC(victim)){
		ch->println("Something went wrong with the pload, victim not found");
		ch->println("Punloading victim.");
		interpret(ch, FORMATF("pload unload %s", argument));
		if(ch->in_room!=rm){
			ch->println("Returning you to old room.");
			char_from_room(ch);
			char_to_room(ch, rm);
		}
		ch->println("UNDENY ABORTED");
		return;
	}

	if(!IS_SET(victim->act, PLR_DENY)){
		ch->println("VICTIM IS NOT CURRENTLY DENIED!");
		ch->println("Punloading victim.");
		interpret(ch, FORMATF("pload unload %s", argument));
		if(ch->in_room!=rm){
			ch->println("Returning you to old room.");
			char_from_room(ch);
			char_to_room(ch, rm);
		}
		ch->println("UNDENY ABORTED");
		return;	
	}

	REMOVE_BIT(victim->act, PLR_DENY);
	ch->println("DENY FLAG REMOVED FROM VICTIM.");

	ch->println("Saving victim.");
	interpret(ch, FORMATF("pload save %s", argument));

	ch->println("Punloading victim.");
	interpret(ch, FORMATF("pload unload %s", argument));
	
	if(ch->in_room!=rm){
		ch->println("Returning you to old room.");
		char_from_room(ch);
		char_to_room(ch, rm);
	}
	ch->println("UNDENY FINISHED");
}

/**************************************************************************/
/**************************************************************************/

