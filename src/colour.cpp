/**************************************************************************/
// colour.cpp - Dawn colour system, (c)1998-2001 Michael Garratt
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"
#include "colour.h"
#include "cust_col.h"
#include "help.h"
#include "clan.h"
#include "socials.h"
#include "interp.h"

#define MAX_RANDOM 14
char randomColours[MAX_RANDOM+2] = "SrRgGyYbBmMcCwW";

bool colour_convert_disabled=true;

//prototypes
int number_range( int from, int to );
/**************************************************************************/
// this table as all the entries the colour table is made from
colour_codes makeFullColourTableFrom[] = {
  // code,special,dont_repeat,noColour,ansi, irc,		ircWhite,		html
  //     (think of dont_repeat as is_colour) 

	// IRC White uses black for `X, `x, `W and `w.  dark gray for `S 
	// it doesn't use light gray (\00314,00)

	//silver
	{'s', false, true, "", "\033[1;30m", "\00314,01", "\00314,00", "</FONT><FONT COLOR=\"#7F7F7F\">"},
	{'S', false, true, "", "\033[1;30m", "\00314,01", "\00314,00", "</FONT><FONT COLOR=\"#7F7F7F\">"},
	//red
	{'r', false, true, "", "\033[0;31m", "\00304,01", "\00304,00", "</FONT><FONT COLOR=\"#FF007F\">"},
	{'R', false, true, "", "\033[1;31m", "\00305,01", "\00305,00", "</FONT><FONT COLOR=\"#FF0000\">"},
	//green
	{'g', false, true, "", "\033[0;32m", "\00303,01", "\00303,00", "</FONT><FONT COLOR=\"#007F00\">"},
	{'G', false, true, "", "\033[1;32m", "\00309,01", "\00309,00", "</FONT><FONT COLOR=\"#00FF00\">"},
	//yellow
	{'y', false, true, "", "\033[0;33m", "\00307,01", "\00308,00", "</FONT><FONT COLOR=\"#7F7F00\">"},
	{'Y', false, true, "", "\033[1;33m", "\00308,01", "\00307,00", "</FONT><FONT COLOR=\"#FFFF00\">"},
	//blue
	{'b', false, true, "", "\033[0;34m", "\00302,01", "\00302,00", "</FONT><FONT COLOR=\"#00007F\">"},
	{'B', false, true, "", "\033[1;34m", "\00312,01", "\00312,00", "</FONT><FONT COLOR=\"#0000FF\">"},
	//magenta
	{'m', false, true, "", "\033[0;35m", "\00306,01", "\00306,00", "</FONT><FONT COLOR=\"#7F007F\">"},
	{'M', false, true, "", "\033[1;35m", "\00313,01", "\00313,00", "</FONT><FONT COLOR=\"#FF00FF\">"},
	//cyan
	{'c', false, true, "", "\033[0;36m", "\00310,01", "\00310,00", "</FONT><FONT COLOR=\"#007F7F\">"},
	{'C', false, true, "", "\033[1;36m", "\00311,01", "\00311,00", "</FONT><FONT COLOR=\"#00FFFF\">"},
	//white
	{'w', false, true, "", "\033[0;37m", "\00315,01", "\00301,00", "</FONT><FONT COLOR=\"#BFBFBF\">"},
	{'W', false, true, "", "\033[1;37m", "\00300,01", "\00301,00", "</FONT><FONT COLOR=\"#FFFFFF\">"},

	//Clear - aliased to white
	{'x', false, true, "", "\033[0;37m", "\00315,01", "\00301,00", "</FONT><FONT COLOR=\"#BFBFBF\">"},
	{'X', false, true, "", "\033[0;37m", "\00315,01", "\00301,00", "</FONT><FONT COLOR=\"#BFBFBF\">"},

	// underlined character - ansi only
	{'_', false, true, "", "\033[4m", "", "", ""},

	// ~ symbol
	{'-', false, false, "~", "~", "~", "~", "~"},
	// ` symbol
	{COLOURCODE, false, false, CCSTR, CCSTR, CCSTR, CCSTR, CCSTR},
	// newline
	{'1', true,  false, "", "", "", "", "<BR>"}, // allowing `1 for newline
	{'}', true,  false, "", "", "", "", "<BR>"}, // allowing `} for newline (historical reasons)
	{'+', false, false, "", "", "", "", ""}, // ignored by colour parser	

	// SPECIAL CODES
	// flashing
	{'F', true,  true, "", "\033[5m", "", "", ""},
	// -- RANDOM COLOUR
	{'?', true,  true, "", "", "", "", ""},
	// -- SAVE
	{'#', true,  true, "", "", "", "", ""},
	// -- RESTORE - NO REWIND
	{'^', true,  true, "", "", "", "", ""},
	// -- RESTORE - AND REWIND
	{'&', true,  true, "", "", "", "", ""},
//	// clear the screen - ansi only - disabled by default
//	{'*', false, true, "", "\033[2J", "", "", ""},

	{'=', true, true, "", "", "", "", ""}, // custom colour pointer 

	// -- mud Name - converted to the name of the mud in the gamesettings.
	{'N', true,  false, "", "", "", "", ""},

	// CUSTOMISE THE OLC COLOURS TO CHANGE THE OLC COLOUR SCHEME
	// olc colour 1
	{'o', false, true, "", "\033[0;33m", "\00305,01", "\00305,00", "</FONT><FONT COLOR=\"#7F7F00\">"},
	// olc colour 2
	{'O', false, true, "", "\033[0;36m", "\00310,01", "\00310,00", "</FONT><FONT COLOR=\"#007F7F\">"},
	// syntax code
	{'.', false, true, "", "\033[1;33m", "\00308,01", "\00308,00", "</FONT><FONT COLOR=\"#FFFF00\">"},

  // code,special,dont_repeat,noColour,ansi, mxp, irc       , ircWhite,       html
  //     (think of dont_repeat as is_colour) 

	// end of table marker
	{'\0', false, false, "", "", "", ""}
};
    
/**************************************************************************/
// empty table now - it is configured when initColour is called
colour_codes colourTable[256] = {
	{'\0', false, false, "", "", "","", ""}

};

/**************************************************************************/
void init_custom_colours();
/**************************************************************************/
// sets up the colour 
void initColour(){
	static bool already_initialised=false;

	// only initialise the system once 
	if(already_initialised){
		log_string("initColour(): Colour system already initialised");
		return;
	}
	already_initialised=true;

	init_custom_colours();

	static colour_codes blankColourEntry;
	int i;
	char buf[20];
	
	// check the colour codes are setup correctly
	sprintf(buf,"%c",COLOURCODE);
	if (strcmp(CCSTR, buf)){
		bugf("initColour: colour codes setup incorrectly!\n");
		bugf("in colour.h CCSTR should be a string version of COLOURCODE\n");
		exit_error( 1 , "initColour", "colour codes setup incorrectly!");
	}

	// first initialise the table 
	for (i=0;i<256; i++){
		colourTable[i]=	blankColourEntry;
	}

	i=0;
	while(makeFullColourTableFrom[i].code!='\0'){
		colourTable[makeFullColourTableFrom[i].code]=makeFullColourTableFrom[i];
		i++;
	}
}
/**************************************************************************/
// return the colour code letter for a particular custom colour code
unsigned char resolve_custom_colour(unsigned char custom_colour_code, COLOUR_MEMORY_TYPE *cm)
{
	// the character after the = is our pointer code
	unsigned char pointer_code=custom_colour_code;

	// find at what position the particular pointer 
	// code is stored in the colour pointer table
	int custom_position=custom_colour_index[pointer_code];
	// get the Custom Colour Code for that position 
	unsigned char ccc=cm->custom_colour[custom_position];
	// if they haven't changed that particular colour use the
	// colour from their underlying template for that position
	if(ccc=='.'){ // . marks no change from template
		ccc=cm->colour_template->template_colour[custom_position];
	}
	// by this stage we have the Custom Colour Code (ccc) 
	// that the user has assigned for a particular custom 
	// colour pointer.

	// if Custom Colour Code is a number, it is a default 
	// template colour, convert into the template value
	if(is_digit(ccc)){ // . marks no change from template
		int default_template=custom_colour_index[ccc];
		ccc=cm->custom_colour[default_template];
		if(ccc=='.'){ // . means use template
			ccc=cm->colour_template->template_colour[default_template];
		}
	}
	return ccc;
}

/**************************************************************************/
#define sz ((int)sizeof(colour_codes))
char *fread_custom_colours(FILE* fp, bool player);
/**************************************************************************/
char helplink_code='\0'; // out here so show_olc_cmd's can hook into it
/**************************************************************************/
// process_colour returns a pointer to a static buffer... has a big buffer, 
// assumes it isnt going to get a buffer overrun... 
// easily big enough for anything Dawn uses at time of coding.
char *process_colour(const char *raw_text, connection_data *d)
{	
	if(IS_NULLSTR(raw_text)){
		return "";
	}

	COLOUR_TYPE cType=CT_HTML;
	COLOUR_MEMORY_TYPE *cm=NULL;
	char_data *ch=NULL;
	bool mxp_client=false;
	bool mxp_secure_prefix_each_line=false;
	bool flashing_disabled=false;
	int gamename_count=0; // can only use gamename a fixed number of times per process colour
	if(d){
		flashing_disabled=d->flashing_disabled;
		cType=d->colour_mode;
		cm=&d->colour_memory;
		mxp_client=d->mxp_enabled;
		if(mxp_client && IS_SET(d->flags, CONNECTFLAG_MXP_SECURE_PREFIX_EACH_LINE)){
			mxp_secure_prefix_each_line=true;
		}else{
			mxp_secure_prefix_each_line=false;
		}
		ch=CH(d);
	}
	char *sp=NULL;

	// static variables - memory allocated first time things are run
	static char *result; // permanantly allocated result buffer
	static COLOUR_MEMORY_TYPE *static_cm=NULL;
	static char brokenhelplink_code='\0';
	if(result==NULL){  // first time run, initialise the statics
		// allocate memory the first time things are run
		logf("process_colour(): Allocating memory for colour processing.");
		result=new char[120120]; 
		// 120120 bytes is used for historical reasons.
		// the old memory management system couldn't allocate a block larger
		// than this... in an ideal world, the code would be rewritten to not 
		// use a buffer like this... but the implementation is acceptable
		// for the time being.
		assertp(result);
		
		// init the static_cm (static colour memory)
		static_cm=new COLOUR_MEMORY_TYPE;
		memset(static_cm, 0, sizeof(COLOUR_MEMORY_TYPE));
		static_cm->current='x';
		memset(static_cm->saved, 'x', MAX_SAVED_COLOUR_ARRAY);
		static_cm->saved_index=0;
		static_cm->custom_colour=fread_custom_colours(NULL, true);

		// allocate the helplink_code (the character used to specify helplinks)
		int i;
		for(i=0; custom_colour_table[i].custom_colour_code!='\0'; i++){
			if(custom_colour_table[i].cc_code==CC_HELP_LINK){
				helplink_code=custom_colour_table[i].custom_colour_code;
				break;
			}
		}
		assert(custom_colour_table[i].custom_colour_code!='\0');
		// helplink_code now contains the '_' character unless it has been changed

		// allocate the brokenhelplink_code (the character used to specify unfound helplinks)
		for(i=0; custom_colour_table[i].custom_colour_code!='\0'; i++){
			if(custom_colour_table[i].cc_code==CC_HELP_BROKENLINK){
				brokenhelplink_code=custom_colour_table[i].custom_colour_code;
				break;
			}
		}
		assert(custom_colour_table[i].custom_colour_code!='\0');
		// brokenhelplink_code now contains the '"' character unless it has been changed
	}

	char *pSrc, *pDest, *pLastNewLine, *baseCol,*col="";

	// get setup for colour conversion
	if(!cm){
		cm=static_cm;
	}

	// assign the defaults to connections without any
	if(cm->colour_template==NULL){ 
		cm->colour_template=default_colour_template;
	}
	if(IS_NULLSTR(cm->custom_colour)){
		cm->custom_colour=fread_custom_colours(NULL, true);
	}

	switch (cType){
		default:
		case CT_NOCOLOUR:
			baseCol= colourTable[0].noColour; break;
		case CT_ANSI:
			baseCol= colourTable[0].ansi; break;
		case CT_IRC:
			baseCol= colourTable[0].irc; break;
		case CT_IRCWHITE:
			baseCol= colourTable[0].ircWhite; break;
		case CT_HTML:
			baseCol= colourTable[0].html; 
			mxp_client=true; // have side effect of converting <'s into &lt; etc
			break;

		case CT_AUTODETECT:
			if(d){
				if(IS_IRCCON(d)){
					baseCol= colourTable[0].ircWhite;
				}else{
					baseCol= colourTable[0].ansi;				
				}
			}else{
				baseCol= colourTable[0].noColour;
			}

			break;

	}

	if(cm->in_help_link){
		pSrc="`=_"; // we get a help link, if we are already in a help link, 
					// switch pSrc over to raw_text when parsing the help link
	}else{
		pSrc=(char *)&raw_text[0];
	}
	pDest=result;
	pLastNewLine=pDest;
	if(mxp_secure_prefix_each_line){
		// automatically put secure prefix for old clients
		sp=MXP_SECURE_LINE;
		while (*sp){ 
			*pDest++=*sp++;
		}
	}

	// convert the colours
	while (*pSrc){
		if (*pSrc==COLOURCODE){
			// check for special codes
			if(colourTable[(unsigned char)*(++pSrc)].special){
				if (*pSrc=='='){ // colour customisation pointer system
					unsigned char ccc;
					pSrc++;
					// If we are processing a custom helplink `=_
					if(*pSrc==helplink_code && mxp_client){
						unsigned char helpcol=0;

						pSrc++; // skip the helplink_code

						// check we dont have a helplink code without any help keyword
						// following cause of end of input... if so we prefix the `=_
						// to the next infomation sent thru the colour system for this 
						// colour memory... bit of a hack but efficient fix
						if(IS_NULLSTR(pSrc)){
							if(cm->in_help_link){
								// last time we had a help link code at the end of a string
								pSrc=(char *)&raw_text[0];
								cm->in_help_link=false;
							}else{
								cm->in_help_link=true;
								continue;
							}
						}
						
						// copy thru any white space directly after the help colour code
						while ( is_space(*pSrc) ){
							*pDest++=*pSrc++;
						}
	
						// now get the help keyword
						char keyword[MIL];
						pSrc=help_find_keyword(pSrc, keyword, ch);
						
						if(help_get_by_keyword(keyword, ch, false)){
							ccc=resolve_custom_colour(helplink_code, cm);
						}else{

							ccc=resolve_custom_colour(brokenhelplink_code, cm);							
						}

						// find out what colour our helplink/broken helplink resolved to
						if (colourTable[(unsigned char)ccc].special)
						{ // it isnt allow to be a special code except random
							if(ccc=='?'){ // random
								cm->current=randomColours[number_range(0, MAX_RANDOM)];
								col=baseCol + ( sz * (int)cm->current );
							}else{
								col="";
							}
							helpcol=cm->current; // dont want to trigger a resend
						}else{
							// dont resend colours we have just sent
							if(cm->current==ccc
								&& colourTable[(unsigned char)ccc].dont_repeat){
								col="";
							}else{
								col=baseCol + ( sz * (unsigned char)ccc);
								// help colours dont affect the current colour since we revert to current after
							}
							helpcol=(unsigned char)ccc;
						}

						strcpy(pDest, col);
						strcat(pDest, "<help>"); // doesn't need to be mxp_tagify'ed cause in process_colour
						strcat(pDest, keyword);
						strcat(pDest, "</help>");

						if(helpcol!=cm->current){ // if the help colour isnt the current colour, revert back
							strcat(pDest, baseCol + ( sz * (unsigned char)cm->current));
						}
						pDest+=str_len(pDest);
						col="";
					}else{
						ccc=resolve_custom_colour(*pSrc, cm);
						// now process the Custom Colour Code (ccc) as if a normal code
						if (colourTable[(unsigned char)ccc].special)
						{ // it isnt allow to be a special code except random
							if(ccc=='?'){ // random
								cm->current=randomColours[number_range(0, MAX_RANDOM)];
								col=baseCol + ( sz * (int)cm->current );
							}else{
								pSrc++;
								continue;
							}
						}else{
							// dont resend colours we have just sent
							if(cm->current==ccc
								&& colourTable[(unsigned char)ccc].dont_repeat){
								pSrc++;
								continue;
							}
							col=baseCol + ( sz * (unsigned char)ccc);
							cm->current=(unsigned char)ccc;
						}
					}
				}else if (*pSrc=='F'){ // flashing, players can turn it off
					if(!flashing_disabled){
						if(	colourTable[(int)*(pSrc)].dont_repeat) // can think of dont_repeat as is_colour
						{
							if(cm->current==*(pSrc)){
								pSrc++;
								continue;
							}
							cm->current=*(pSrc); // record the current colour only if it is a colour
						}
						col=baseCol + ( sz * (int)*pSrc );
					}else{
						col="";

					}
				}else if (*pSrc=='N'){ // game/mud Name
					if(++gamename_count<10){
						col=game_settings->gamename;
					}else{
						col="";
					}
				}else if (*pSrc=='?'){ // random
					cm->current=randomColours[number_range(0, MAX_RANDOM)];
					col=baseCol + ( sz * (int)cm->current );
				}else if (*pSrc=='#'){ // save current colour using a save buffer
					++cm->saved_index%=MAX_SAVED_COLOUR_ARRAY;
					cm->saved[cm->saved_index]=cm->current;
					col="";
				}else if (*pSrc=='^'){ // restore colour, no rewind of save buffer
					cm->current=cm->saved[cm->saved_index];
					col=baseCol + ( sz * ((int)cm->current));				
				}else if (*pSrc=='&'){ // restore colour and rewind of save buffer
					cm->current=cm->saved[cm->saved_index];
					col=baseCol + ( sz * ((int)cm->current));
					cm->saved_index=(cm->saved_index+(MAX_SAVED_COLOUR_ARRAY-1))%MAX_SAVED_COLOUR_ARRAY;
				}else if (*pSrc=='1' || *pSrc==/*{*/'}'){ // newline 
					if(cType!=CT_HTML){
						*pDest++='\r'; // all but html have the \r
					}
					*pDest++='\n';

					// we back up the pointer to the character immediately following the last '\n'
					pLastNewLine=pDest;
					if(mxp_secure_prefix_each_line){
						// automatically put secure prefix for old clients
						sp=MXP_SECURE_LINE;
						while (*sp){ 
							*pDest++=*sp++;
						}
					}

					if(cType==CT_IRC || cType==CT_IRCWHITE){// irc has to have the previous 																	
						col=baseCol + ( sz * ( (int)cm->current)); // lines colour resent
					}else{
						col="";
					}
				}
			}else{ // not a special code - (colourTable[(int)*(++pSrc)].special)=false
				// normal code
				// dont resend colours we have already changed to
				if(	colourTable[(int)*(pSrc)].dont_repeat) // can think of dont_repeat as is_colour
				{
					if(cm->current==*(pSrc)){
						pSrc++;
						continue;
					}
					cm->current=*(pSrc); // record the current colour only if it is a colour
				}
				col=baseCol + ( sz * (int)*pSrc );
			}

			// by this stage, the colour code has been processed, and 
			// 'char *col' is pointing at the result of the code

			// copy 'col' into the destination
			while (*col){ 
				*pDest++=*col++;
			}

			if(*pSrc){ // move onto the next source character 	
				pSrc++;// (if we arent at the end of the string)
			}
		}else if (mxp_client){ // we didn't have a colour code, but checks for HTML coding			
			switch (*pSrc){
			default: *pDest++=*pSrc++;	break;

			case '\r':	// strip off \r in HTML
				if(cType==CT_HTML){
					pSrc++;  
				}else{
					*pDest++=*pSrc++;
				}
				break; 

			case '\n': // record where the end of the line is so we can insert stuff after it
				*pDest++=*pSrc++;
				pLastNewLine=pDest;
				if(mxp_secure_prefix_each_line){
					// automatically put secure prefix for old clients
					sp=MXP_SECURE_LINE;
					while (*sp){ 
						*pDest++=*sp++;
					}
				}
				break;

			case '&': // convert '&' symbol into correct html code
				*pDest++='&';
				*pDest++='a';
				*pDest++='m';
				*pDest++='p';
				*pDest++=';';
				pSrc++;
				break;

			case '<': // convert '<' symbol into correct html code
				*pDest++='&';
				*pDest++='l';
				*pDest++='t';
				*pDest++=';';
				pSrc++;
				break;

			case '>': // convert '>' symbol into correct html code
				*pDest++='&';
				*pDest++='g';
				*pDest++='t';
				*pDest++=';';
				pSrc++;
				break;

			case MXP_AMPERSAND: // convert mxp_ampersand into symbol '&'
				*pDest++='&';
				pSrc++;
				break;

			case MXP_BEGIN_TAG: // convert mxp_begin_tag into symbol '<'
				*pDest++='<';
				pSrc++;
				break;

			case MXP_END_TAG: // convert mxp_end_tag into symbol '>'
				*pDest++='>';
				pSrc++;
				break;

#ifdef VALIDATE_HTML
			case ' ': // convert ' ' into a space in HTML
				*pDest++='&';
				*pDest++='#';
				*pDest++='1';
				*pDest++='6';
				*pDest++='0';
				*pDest++=';';
				pSrc++;
				break;
#endif
			}				
		}else if(*pSrc=='\n'){ // do special things at the end of the line?
			*pDest++=*pSrc++; // copy the '\n' character
			pLastNewLine=pDest;
			if(mxp_secure_prefix_each_line){
				// automatically put secure prefix for old clients
				sp=MXP_SECURE_LINE;
				while (*sp){ 
					*pDest++=*sp++;
				}
			}


			if (cType==CT_IRC || cType==CT_IRCWHITE){
				// resend colour on new lines
				col=baseCol + ( sz * ( (int)cm->current));
				while (*col){
					*pDest++=*col++;
				}
			}
		}else{
			*pDest++=*pSrc++; // copy plain characters to the destination
		}

	}
	// terminate the string
	*pDest='\0';

	// return the length
	return (result);
}
/**************************************************************************/
// convertColour
// returns the length of dest - the colour coded string
// - This function is old, and was the original basis for the webserver
//   colour parsing system... Only the webhelp and 
//   write_to_descriptor_colour() still use it... since I dont have time 
//   to remove it from the source before the Dawn 1.7 release it remains...
//   - Kalahn, September 2000.
int convertColour(const char *src, char *dest, COLOUR_TYPE cType, bool parital_html){
	char *pSrc, *pDest, *baseCol,*col="";

	char savedCol='x';
	char currentCol='\0';
	bool flashing_disabled=false;

	switch (cType){
		default:
		case CT_NOCOLOUR:
			baseCol= colourTable[0].noColour; break;
		case CT_ANSI:
			baseCol= colourTable[0].ansi; break;
		case CT_IRC:
			baseCol= colourTable[0].irc; break;
		case CT_IRCWHITE:
			baseCol= colourTable[0].ircWhite; break;
		case CT_HTML:
			baseCol= colourTable[0].html; break;
	}

	// convert the colours
	pSrc=(char *)&src[0];
	pDest=dest;
	while (*pSrc){
		if (*pSrc==COLOURCODE){
			// check for special codes
			if (colourTable[(int)*(++pSrc)].special){
				if (*pSrc=='='){ // colour customisation pointer system
					// the character after the = is our pointer code
					unsigned char pointer_code=*(++pSrc);

					// find at what position the particular pointer 
					// code is stored in the colour pointer table
					int custom_position=custom_colour_index[pointer_code];
					// get the Custom Colour Code from the default template
					unsigned char ccc=default_colour_template->template_colour[custom_position];
					// by this stage we have the default Custom Colour Code (ccc) 

					// if Custom Colour Code is a number, it is a default 
					// template colour, convert into the template value
					if(is_digit(ccc)){ // . marks no change from template
						int default_template=custom_colour_index[ccc];
						ccc=default_colour_template->template_colour[default_template];
					}

					// now process the Custom Colour Code (ccc) as if a normal code
					if (colourTable[(unsigned char)ccc].special)
					{ // it isnt allow to be a special code except random
						if(ccc=='?'){ // random
							currentCol=randomColours[number_range(0, MAX_RANDOM)];
							col=baseCol + ( sz * (int)currentCol);
						}else{
							pSrc++;
							continue;
						}
					}else{
						// dont resend colours we have just sent
						if(currentCol==ccc
							&& colourTable[(unsigned char)ccc].dont_repeat){
							pSrc++;
							continue;
						}
						col=baseCol + ( sz * (unsigned char)ccc);
						currentCol=(unsigned char)ccc;
					}
				}else if (*pSrc=='F'){ // flashing, players can turn it off
					if(!flashing_disabled){
						if(	colourTable[(int)*(pSrc)].dont_repeat) // can think of dont_repeat as is_colour
						{
							currentCol=*(pSrc);
							col=baseCol + ( sz * (int)currentCol );
						}
						col=baseCol + ( sz * (int)*pSrc );
					}else{
						col="";

					}
				}else if (*pSrc=='?'){ // random
					currentCol=randomColours[number_range(0, MAX_RANDOM)];
					col=baseCol + ( sz * (int)currentCol );
				}else
				// save
				if (*pSrc=='#'){
					savedCol=currentCol;
					col="";
				}else
				//restore
				if (*pSrc=='^'){
					col=baseCol + ( sz * ( (int)savedCol ));
					currentCol=savedCol;
				}else if //{
					(*pSrc=='1' || *pSrc=='}'){ // newline 
					if(cType!=CT_HTML){
						*pDest++='\r'; // all but html have the \r
					}
					*pDest++='\n';

					// we back up the pointer to the character immediately following the last '\n'
//					pLastNewLine=pDest;

					if(cType==CT_IRC || cType==CT_IRCWHITE){// irc has to have the previous 																	
						col=baseCol + ( sz * ( (int)savedCol)); // lines colour resent
					}else{
						col="";
					}
				}
			}else{
				// normal code

				// dont repeat displayed codes				
				if(colourTable[(int)*(pSrc)].dont_repeat 
					&& currentCol==*(pSrc)){
					pSrc++;
					continue;
				}
				currentCol=*(pSrc);
				col=baseCol + ( sz * (int)currentCol );
			}
			while (*col){
				*pDest++=*col++;
			}
			if(*pSrc){
				pSrc++;
			}
		}else if (cType==CT_HTML){
			switch (*pSrc){
				default:
					*pDest++=*pSrc++;
				break;
			
			case '\r': // strip off \r in HTML
				pSrc++;
				break;

/*			case '\n': // convert \n into <BR> in HTML
				*pDest++='<';
				*pDest++='B';
				*pDest++='R';
				*pDest++='>';
#ifdef VALIDATE_HTML
				*pDest++='\n';
#endif
				pSrc++;
				break;
*/

			case '&': // convert '&' symbol into correct html code if not parital html
				if(parital_html){
					*pDest++=*pSrc++;
				}else{
					*pDest++='&';
					*pDest++='a';
					*pDest++='m';
					*pDest++='p';
					*pDest++=';';
					pSrc++;
				}
				break;
				
			case '>': // convert '>' symbol into correct html code if not parital html
				if(parital_html){
					*pDest++=*pSrc++;
				}else{
					*pDest++='&';
					*pDest++='#';
					*pDest++='6';
					*pDest++='2';
					*pDest++=';';
					pSrc++;
				}
				break;

			case '<': // convert '<' symbol into correct html code if not parital html
				if(parital_html){
					*pDest++=*pSrc++;
				}else{
					*pDest++='&';
					*pDest++='#';
					*pDest++='6';
					*pDest++='0';
					*pDest++=';';
					pSrc++;
				}
				break;

#ifdef VALIDATE_HTML
			case ' ': // convert ' ' into a space in HTML
				*pDest++='&';
				*pDest++='#';
				*pDest++='1';
				*pDest++='6';
				*pDest++='0';
				*pDest++=';';
				pSrc++;
				break;
#endif
			}
		}else if (cType==CT_IRC || cType==CT_IRCWHITE){
			switch (*pSrc){
				default:
					*pDest++=*pSrc++;
					break;
			
				case '\n': // resend colour on new lines
					*pDest++=*pSrc++;
					col=baseCol + ( sz * ( (int)currentCol ));
					while (*col){
						*pDest++=*col++;
					}
					break;
			}
		}else{
			*pDest++=*pSrc++;
		}

	}
	// terminate the string
	*pDest='\0';

	// return the length
	return (int)(pDest-dest);
}
/**************************************************************************/
// Function: strip_colour() - Kal October 2000
// Notes: Uses convertColour to parse all colour codes out, pretty simple
char *strip_colour(const char *coloured_text)
{ 
	static int mri; // multi result index
	static char *multi_result[3]; // circular managed result buffer 
	// nothing to do with empty strings
	if( IS_NULLSTR(coloured_text)){
        return "";
	}
	// rotate buffers
	++mri%=3;

	manage_dynamic_buffer(&multi_result[mri], str_len(coloured_text)+MSL); // maintain result so always has enough space
	char *result=multi_result[mri]; // managed result buffer

	convertColour(coloured_text, result, CT_NOCOLOUR, false);

	return result;
}
/***************************************************************************/
const struct colour_table_type colour_table[]=
{
    {"`x"},	//  0
    {"`r"},	//  1
    {"`g"},	//  2
    {"`y"},	//  3
    {"`b"},	//  4
    {"`m"},	//  5
    {"`c"},	//  6
    {"`w"},	//  7
    {"`s"},	//  8
    {"`R"},	//  9
    {"`G"},	// 10
    {"`Y"},	// 11
    {"`B"},	// 12
    {"`M"},	// 13
    {"`C"},	// 14
    {"`W"},	// 15
};
/**************************************************************************/
// converts the use of { colour codes to ` in a string
// writes into a result buffer which is returned
char *colour_convert_code_format(char *text)
{
	static char *result; // managed result buffer
	// nothing to do with empty strings
	if( IS_NULLSTR(text)){
        return "";
	}
	manage_dynamic_buffer(&result, str_len(text)*2+1); // maintain result so always has enough space

	// convert ` -> ``
	// convert {{ -> {
	// convert {` -> `?
	// convert {} -> `1
	// convert {=x to `=x (where x represents any character but nul)
	// convert {  -> `

	char *d=result;
	for(char *p=text; *p; p++){
		if(*p=='`'){ // convert ` into ``
			*d++='`';
			*d++='`';
			continue;
		}else if(*p!='{'){ // we haven't found an oldstyle colour code
			*d++=*p;
			continue;
		}
		// { oldstyle colour code discovered, work on conversion code
		p++; // skip to the character after the { code
		if(!*p){ // check that we aren't looking at a NULL
			break; // if we have terminate the loop
		}

		// convert {{ -> {
		if(*p=='{'){
			*d++='{';
			continue;
		}

		// convert {` -> `?
		if(*p=='`'){
			*d++='`';
			*d++='?';
			continue;
		}

		// convert {} -> `1
		if(*p=='}'){ // newline, new encouraged format
			*d++='`';
			*d++='1';
			continue;
		}

		// convert {=x -> `=x (where x is anything but null)
		if(*p=='='){
			p++; // skip to the character after the = custom colour code character
			if(!*p){ // check that we aren't looking at a NULL
				break; // if we have terminate the loop
			}

			*d++='`';
			*d++='=';
			*d++=*p;
			continue;
		}

		// not a special code, copy it over
		*d++='`';
		*d++=*p; 
	}
	*d='\0'; // terminate the result

	return result;
}
/**************************************************************************/
#define str_replace_colour_code(str) \
	do{   char *t; \
		if((str)!=NULL && (str)[0]!='\0'){ \
		t=colour_convert_code_format(str); \
			replace_string(str, t); \
		} \
	}while(0)

/**************************************************************************/
void colour_convert_helps( helpfile_data *pHelpfile )
{
	if(colour_convert_disabled)return;

	if(pHelpfile->colourcode==COLOURCODE){
		// no need to convert the colour codes for this help
		// since they have already been done
		return;
	}
	logf(" >>>colour_convert_helps(%s)", pHelpfile->file_name);
	// *** convert helps 
	for ( help_data *pHelp = help_first; pHelp; pHelp = pHelp->next )
	{
		if(pHelp->helpfile!=pHelpfile){
			continue;
		}
		// help text
		str_replace_colour_code(pHelp->text);

		// help title
		str_replace_colour_code(pHelp->title);

		// help keywords
		str_replace_colour_code(pHelp->keyword);

		// now strip all colour from the keywords
		char *t=strip_colour(pHelp->keyword);
		replace_string(pHelp->keyword,t);
	}
	// mark the help as converted
	pHelpfile->colourcode=COLOURCODE;	
}

/**************************************************************************/
extern bool fBootDb;
/**************************************************************************/
void colour_convert_area( AREA_DATA *area)
{
	if(colour_convert_disabled)return;

	int i;
	EXTRA_DESCR_DATA *pEd;

	if(area->colourcode==COLOURCODE){
		// no need to convert the colour codes for this area
		// since they have already been done
		return;
	}
	logf(" >>>colour_convert_area(%s) [%d-%d]", area->name, area->min_vnum, area->max_vnum);
	
	// first set fBootDb to false to disable any logging of unfound 
	// mobiles in get_mob_index() and rooms in get_room_index()
	bool backup_fBootDb=fBootDb;
	fBootDb=false; 

	// *** convert all mobiles in an area
    for( i = area->min_vnum; i <= area->max_vnum; i++ )
    {
		MOB_INDEX_DATA *pMob;
		if ( (pMob = get_mob_index( i )) ){
			str_replace_colour_code(pMob->short_descr);
			str_replace_colour_code(pMob->long_descr );
			str_replace_colour_code(pMob->description);
			str_replace_colour_code(pMob->material); 
		}
    }

	// *** convert all objects in an area
	for( i = area->min_vnum; i <= area->max_vnum; i++ )
    {
		OBJ_INDEX_DATA *pObj;
		if ( (pObj = get_obj_index( i )) ){
			str_replace_colour_code(pObj->short_descr);
			str_replace_colour_code(pObj->description);
			str_replace_colour_code(pObj->material); 
			// convert any object extra descriptions
			for(pEd=pObj->extra_descr; pEd; pEd=pEd->next){
				str_replace_colour_code(pEd->description);
			}
		}
    }

	// *** convert all rooms in the area
    for( i = area->min_vnum; i <= area->max_vnum; i++ ){
		ROOM_INDEX_DATA *pRoomIndex=get_room_index(i);
		if(pRoomIndex){
			str_replace_colour_code(pRoomIndex->name);
			str_replace_colour_code(pRoomIndex->description);
			
			{ // convert any room echo colour code 
				room_echo_data *pRe;
				for(pRe=pRoomIndex->echoes; pRe; pRe=pRe->next){
					str_replace_colour_code(pRe->echotext);
				}
			}

			// convert any room extra descriptions
			for(pEd=pRoomIndex->extra_descr; pEd; pEd=pEd->next){
				str_replace_colour_code(pEd->description);
			}

			// convert any exits leading out of the room
			for( int exit= 0; exit<MAX_DIR; exit++)
			{
				EXIT_DATA *pExit;
				if ((  pExit = pRoomIndex->exit[exit] )
					&& pExit->u1.to_room )
				{
					if(!IS_NULLSTR(pExit->description)){
						str_replace_colour_code(pExit->description);
					}
                }
            }
		}
	}

	// *** convert all mobprogs in the area
	{
		MPROG_CODE *pMprog;	
		for( i = area->min_vnum; i <= area->max_vnum; i++ ){
			if ( (pMprog = get_mprog_index(i) )){
				str_replace_colour_code(pMprog->author);
				str_replace_colour_code(pMprog->last_editor);
				str_replace_colour_code(pMprog->code);  
				str_replace_colour_code(pMprog->disabled_text);
				str_replace_colour_code(pMprog->title);
			}
		}
	}

	// *** convert colour codes in area header
	str_replace_colour_code(area->name);
	str_replace_colour_code(area->builders);
	str_replace_colour_code(area->short_name);
	str_replace_colour_code(area->credits);
	if(!IS_NULLSTR(area->lcomment)){
		str_replace_colour_code(area->lcomment);
	}
			
	// mark the area as converted
	area->colourcode=COLOURCODE;

	// restore the logging
	fBootDb=backup_fBootDb; 
}

/**************************************************************************/
void do_save_gamesettings(char_data *ch, char *);
extern CClanType *clan_list;
void save_clan_db( void );
/**************************************************************************/
void colour_convert_player_object( obj_data *obj)
{
	if(colour_convert_disabled)return;

	EXTRA_DESCR_DATA *pEd;
	EXTRA_DESCR_DATA *pEdParent;
	if ( obj->next_content ){
		colour_convert_player_object(obj->next_content);
	}

	if(obj->pIndexData){
		str_replace_colour_code(obj->owner);
		if(str_cmp(obj->name, obj->pIndexData->name)){
			str_replace_colour_code(obj->name);
		}
		if(str_cmp(obj->short_descr, obj->pIndexData->short_descr)){
			str_replace_colour_code(obj->short_descr);
		}
		if(str_cmp(obj->description, obj->pIndexData->description)){
			str_replace_colour_code(obj->description);
		}
		if(str_cmp(obj->material, obj->pIndexData->material)){
			str_replace_colour_code(obj->material);
		}

		// convert any room extra descriptions
		bool found;
		for(pEd=obj->extra_descr; pEd; pEd=pEd->next){
			found=false;
			for(pEdParent=obj->pIndexData->extra_descr; pEdParent; pEdParent=pEdParent->next){
				if(!str_cmp(pEdParent->keyword, pEd->keyword)){
					if(!str_cmp(pEdParent->description, pEd->description)){
						found=true; // if we have a description that doesn't need converting
					}
					break;
				}
			}
			if(!found){
				str_replace_colour_code(pEd->description);
			}
		}
	}

    if ( obj->contains ){
		colour_convert_player_object(obj->contains);
	}
}

/**************************************************************************/
void colour_convert_player( char_data *ch)
{
	if(colour_convert_disabled)return;

	int i;
	if(!ch || !ch->pcdata || ch->pcdata->colour_code==COLOURCODE){
		return;
	}

	logf(" >>>colour_convert_player(%s)", ch->name);

	str_replace_colour_code(ch->short_descr);
	str_replace_colour_code(ch->long_descr);
	str_replace_colour_code(ch->description);
	str_replace_colour_code(ch->gprompt);
	str_replace_colour_code(ch->prompt);
	str_replace_colour_code(ch->olcprompt);
	str_replace_colour_code(ch->prefix);
	str_replace_colour_code(ch->wiznet_colour[0]);
	str_replace_colour_code(ch->wiznet_colour[1]);
	str_replace_colour_code(ch->wiznet_colour[2]);
	str_replace_colour_code(ch->wiznet_colour[3]);
	str_replace_colour_code(ch->material);

	str_replace_colour_code(ch->pcdata->bamfin);
	str_replace_colour_code(ch->pcdata->bamfout);
	str_replace_colour_code(ch->pcdata->fadein);
	str_replace_colour_code(ch->pcdata->fadeout);
	str_replace_colour_code(ch->pcdata->surname);
	str_replace_colour_code(ch->pcdata->birthplace);
	str_replace_colour_code(ch->pcdata->haircolour);
	str_replace_colour_code(ch->pcdata->eyecolour);
	for(i=0; i<9; i++){
		str_replace_colour_code(ch->pcdata->trait[i]);
	}
	str_replace_colour_code(ch->pcdata->crest);
	for(i=0; i<MAX_ALIAS; i++){
		str_replace_colour_code(ch->pcdata->alias[i]);
		str_replace_colour_code(ch->pcdata->alias_sub[i]);
	}
	str_replace_colour_code(ch->pcdata->webpage);
	str_replace_colour_code(ch->pcdata->charnotes);
	str_replace_colour_code(ch->pcdata->who_text);
	str_replace_colour_code(ch->pcdata->afk_message);
	str_replace_colour_code(ch->pcdata->title);
	str_replace_colour_code(ch->pcdata->immtitle);
	str_replace_colour_code(ch->pcdata->immtalk_name); // for morts with immtalk
	str_replace_colour_code(ch->pcdata->letter_workspace_text); // The text of a letter in progress

	// convert the objects they are carrying
	if(ch->carrying){
		colour_convert_player_object(ch->carrying);
	}
	
	ch->pcdata->colour_code=COLOURCODE;

}
/**************************************************************************/
void colour_convert_prefix(char colcode, char *text)
{
	char buf[MIL*2];
	bool moved=false;
	char *src=text;
	char *dest=text;

	assert(colcode!=COLOURCODE); // it is a programming bug if we are converting for no reason

	for(; *src; ){

		// we need to handle the case where we have to convert a ` character to ``
		if(*src==COLOURCODE){
			if(!moved){
				strcpy(buf, src);
				src=buf;
				moved=true;
			}
			*dest++=*src;
			*dest++=*src++;
			continue;
		}

		if(*src==colcode){
			src++; // what comes after a players designated colour prefix is what counts
			// double up of that prefix convert to that prefix

			// make sure we dont have a nul after their colour prefix
			if(*src=='\0'){
				*dest++=COLOURCODE;
				*dest='\0';
				return;
			};

			// handle the use of prefix double up to get a single character			
			if(*src==colcode){
				*dest++=*src++;
				continue;
			}

			if(*src=='}'){ // convert } -> 1
				*dest++=COLOURCODE;
				*dest++='1';
				src++;
				continue;
			}
			
			if(*src=='`'){ // convert old random code ` -> ?
				*dest++=COLOURCODE;
				*dest++='?';
				src++;
				continue;
			}

			// normal use of colour code
			*dest++=COLOURCODE;
			*dest++=*src++;
			continue;
		}

		// normal character
		*dest++=*src++;
	}

	// terminate the string
	*dest='\0';

}
/**************************************************************************/
// colour convert gamesettings
void colour_convert_gamesettings( )
{
	if(colour_convert_disabled)return;

	if(IS_SET(game_settings->uneditable_flags,GAMESETUNEDIT_MANUAL_COLOUR_CONVERT_PERFORMED)){
		// no need to convert the settings if they have already been done
		return;
	}

	log_string(" >>>colour_convert_gamesettings");

	str_replace_colour_code(game_settings->gamename);
	str_replace_colour_code(game_settings->login_prompt);

	// mark the gamesettings as converted
	SET_BIT(game_settings->uneditable_flags,GAMESETUNEDIT_MANUAL_COLOUR_CONVERT_PERFORMED);
	do_save_gamesettings(NULL, ""); // resave so default vnums can be manually edited
}
/**************************************************************************/
void note_convert_colour_codes(); // in note.cpp
/**************************************************************************/
void colour_convert_notes( )
{
	if(colour_convert_disabled)return;

	log_string(" >>>colour_convert_notes");

	note_convert_colour_codes();
	// notes are resaved within note_convert_colour_codes()'s code
}
/**************************************************************************/
void colour_convert_socials()
{
	if(colour_convert_disabled)return;

	logf("===colour_convert_socials()");

	// patch up the NULL pointers for str_dup("");
	// One day might even sort the list alphabetically, but not today :)
	social_count=0;
	for(social_type *soc=social_list; soc; soc=soc->next){
		social_count++;
		for(int i=0; i<SOCIAL_ATMAX; i++){
			str_replace_colour_code(soc->acts[i]);
		}
	}

	// resave the socials
	save_socials();
}
/**************************************************************************/
void colour_convert_clans( )
{		
	if(colour_convert_disabled)return;

	log_string(" >>>colour_convert_clans");

    CClanType *pClan;
	for(pClan=clan_list;pClan;pClan=pClan->next){
		str_replace_colour_code(pClan->m_pColorStr);
		str_replace_colour_code(pClan->m_pWhoName);
	}

	// resave the clan database
	save_clan_db();
}
/**************************************************************************/
int command_exact_lookup( const char *name );
/**************************************************************************/
// this function should only be run ONCE!
void do_manual_colour_convert(char_data *ch, char *arg)
{
	if(colour_convert_disabled)return;

	if(IS_SET(game_settings->uneditable_flags,GAMESETUNEDIT_MANUAL_COLOUR_CONVERT_PERFORMED)){
		ch->println("It appears that manual_colour_convert has already been used.");		
		return;
	}
	if(str_cmp(arg,"confirm")){		
		ch->println("manual_colour_convert should only be used by muds upgrading from 1.69p and lower!");
		ch->println("Type 'manual_colour_convert confirm'");		
		return;
	}
	ch->println("Doing manual colour conversion of:");
	logf("starting manual_colour_convert()");
	ch->println("game settings");
	colour_convert_gamesettings();
	ch->println("---notes");
	colour_convert_notes();
	ch->println("---socials");
	colour_convert_socials();
	ch->println("---clans");
	colour_convert_clans();
	logf("finished manual_colour_convert()");
	ch->println("completed conversion.");
	
	SET_BIT(game_settings->uneditable_flags,GAMESETUNEDIT_MANUAL_COLOUR_CONVERT_PERFORMED);
	do_save_gamesettings(NULL, ""); // resave so default vnums can be manually edited

	int i = command_exact_lookup( "manual_colour_convert");
	if(i>=0){
		ch->println("Setting manual_colour_convert command to above MAX_LEVEL.");
		cmd_table[i].level=MAX_LEVEL+2;
		do_write_commandtable(ch,"");
		ch->println("Finished removing the manual_colour_convert command.");
	}
}
/**************************************************************************/
/**************************************************************************/

