/**************************************************************************/
// mob_q.cpp - Mob queue system - Kalahn
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#include "include.h"

int run_mpqueue_count=0;
char *mq_mpbug_details;
bool mq_running_queued_command=false;
/**************************************************************************/
#define MQFLAG_SEEALL (A)
/**************************************************************************/
enum mobqueuetype {MQT_UNDEFINED, MQT_COMMAND, 
	MQT_PRINT, MQT_ECHOAT, MQT_ECHOAROUND, MQT_ECHOROOM};

/**************************************************************************/
class mpqueue_type{
public:
	char_data* ch;
	time_t when; // when the prog will go off (when current_time equals this)
	mobqueuetype type;
	int flags;
	char *text;
	mpqueue_type *next; // next in hash bucket]
	char *mpbug_details;

public: // public member functions
	mpqueue_type();// default constructor
	mpqueue_type(mobqueuetype t, time_t seconds, char_data * mob, const char *command);// custom constructor
	~mpqueue_type();// deconstructor
};
#define MPQUEUE_HASH_SIZE (256)

/**************************************************************************/
// default constructor
mpqueue_type::mpqueue_type()
{
	flags=0;
	type=MQT_UNDEFINED;
	ch=NULL;
	text=str_dup("");
	when=0;
	next=NULL;
};
/**************************************************************************/
// default destructor
mpqueue_type::~mpqueue_type()
{
	if(next){
		bug("Warning: mpqueue_type::~mpqueue_type() called when next!=NULL!!!");
	}
	free_string(text);
};
/**************************************************************************/
// default constructor
mpqueue_type::mpqueue_type(mobqueuetype t, time_t seconds, char_data * mob, const char *argument)
{
	flags=0;
	type=t;
	text=str_dup(argument);
	when=current_time +seconds;
	ch=mob;
	ch->mpqueue_attached(when);
	next=NULL;
};

/**************************************************************************/
int mpqueued_count=0;
int mpqueued_total=0;
mpqueue_type *mpqueue_hash_table[MPQUEUE_HASH_SIZE];
/**************************************************************************/
void mpqueue_dequeue_for(char_data *extracted_char)
{
	char *tbuf;
	if(run_mpqueue_count>0){
		// can't remove nodes while in run_mpqueue() 
		// otherwise it creates crashes later on since run_mpqueue() performs some 
		// dequeueing itself (good bug find :) ) - Kal April 00 
		logf("mpqueue_dequeue_for('%s' %d) called with run_mpqueue() on call stack - NULLing node->ch", extracted_char->name, extracted_char->vnum());
		mpqueue_type *node;
		for(int h=0; h<MPQUEUE_HASH_SIZE; h++){
			for(node=mpqueue_hash_table[h]; node; node=node->next){
				if(node->ch==extracted_char){
					tbuf=ctime( &node->when);
					tbuf[str_len(tbuf)-6] = '\0';
					logf("###############NULLed deque node->ch '%s' at %s, queue system will cleanup later", node->text, tbuf+4);
					node->ch=NULL;
				}
			};
		}
	}else{
		logf("mpqueue_dequeue_for('%s' %d)", extracted_char->name, extracted_char->vnum());
		mpqueue_type *node, *node_next, *prev;
		for(int h=0; h<MPQUEUE_HASH_SIZE; h++){

			prev=NULL;
			for(node=mpqueue_hash_table[h]; node; node=node_next){
				node_next=node->next;
				if(node->ch==extracted_char){
					tbuf=ctime( &node->when);
					tbuf[str_len(tbuf)-6] = '\0';
					logf("###############deque node '%s' at %s", node->text, tbuf+4);
					node->next=NULL;
					delete node;
					// remove node from the list
					if(prev){
						prev->next=node_next;
					}else{
						mpqueue_hash_table[h]=node_next;
					}
					mpqueued_count--; // stats stuff
				}else{ // a node has been kept, therefore it is the previous node
					prev=node;
				}
			};
		}
	}
}
/**************************************************************************/
void do_mpdequeueall(char_data *ch, char *)
{
	mpqueue_dequeue_for(ch);
}
/**************************************************************************/
// execute a bucket in the hash table queue
void run_mpqueue(mpqueue_type *parent)
{	
	run_mpqueue_count++;

	mpqueue_type *node=parent->next;
	assertp(node);

	if(node->when>current_time){ // only run once it is due to be run
		run_mpqueue_count--;
		return;
	}
	if(node->next){ // run nodes futher down
		run_mpqueue(node);		
	}

	if(node->ch){
		switch(node->type){
			case MQT_COMMAND:
					interpret(node->ch, node->text);
				break;

			case MQT_PRINT:
				node->ch->set_pdelay(0); // incase someone stuffed up and we would get an endless loop
				node->ch->print(node->text);
				break;

			case MQT_ECHOAT:
				act(node->text, node->ch, NULL, NULL, TO_CHAR);
				break;

			case MQT_ECHOAROUND:
				act(node->text, node->ch, NULL, NULL, TO_ROOM);
				break;

			case MQT_ECHOROOM:
				act(node->text, node->ch, NULL, NULL, TO_CHAR);
				act(node->text, node->ch, NULL, NULL, TO_ROOM);
				break;
				
			default:
				bug("run_mpqueue(mpqueue_type *parent): Unsupported recognised queued event type!");
				break;
		}
	}else{ // NULL node->ch
		logf("###############NULL node->ch found '%s', queue system deleting", node->text);
	}
	// dequeue it node from the list
	parent->next=node->next;
	node->next=NULL;
	delete node;

	// statistics stuff
	mpqueued_count--;
	run_mpqueue_count--;
}
/**************************************************************************/
// called every pulse - could be cleaner but who cares :)
void process_mpqueue()
{	
	// run optimization and table initialisation 
	static time_t last_run=0;
	if(last_run==current_time){
		return;
	}
	if(last_run==0){ // initialise the table (first time)
		memset(&mpqueue_hash_table[0],0,sizeof(mpqueue_type*)*MPQUEUE_HASH_SIZE);		
	}

	// find what we are running this second - if anything
	last_run=current_time;
	int hash=(int)current_time%MPQUEUE_HASH_SIZE;
	
	if( !mpqueue_hash_table[hash] || mpqueue_hash_table[hash]->when>current_time){
		return; // nothing to run this time
	}

	// run it
	static mpqueue_type *parent=NULL;if(!parent){parent=new mpqueue_type;} // get a parent variable
	parent->next=mpqueue_hash_table[hash];
	run_mpqueue(parent);
	mpqueue_hash_table[hash]=parent->next;
	return;
}
/**************************************************************************/
// syntax: mob queue <seconds> command to run 
void do_mpqueue(char_data *ch, char *argument)
{
	char seconds_arg[MIL];
	char fullarg[MIL];
	strcpy(fullarg, argument);
	
	argument=one_argument(argument,seconds_arg);

	// validate arguments
	if(IS_NULLSTR(argument)){ // not enough arguments
		mpbug("do_mpqueue: not enough arguments, must have at least seconds and a command to queue.");
		mpbug("syntax: mob queue <seconds> command to type )");
		return;
	}
	if(!is_number(seconds_arg)){
		mpbug("do_mpqemote: <seconds> ('%s') must be numeric.",  seconds_arg);
		mpbug("syntax: mob queue <seconds> command to type )");
		return;
	}
	
	// START PROCESSING AND VALIDATING ARGUMENTS
	// Number of seconds in which prog will be run
	int seconds=atoi(seconds_arg);
	if(seconds<0 || seconds>3000){
		mpbug("do_mpqueue: seconds parameter must be between 1 and 3000.");
		mpbug("syntax: mob queue <seconds> command to type )");
		mpbug("buggy argument: '%s'", fullarg);
		return;
	}

	if(!IS_VALID(ch)){
		mpbug("A deleted mob is trying to queue '%s'.", fullarg);
		return;
	}

	mpqueue_type *node=new mpqueue_type(MQT_COMMAND, seconds, ch, argument);
	int hash=(int)node->when%MPQUEUE_HASH_SIZE;

	// queue it into the hash table buckets
	node->next=mpqueue_hash_table[hash];
	mpqueue_hash_table[hash]=node;

	// statistics stuff
	mpqueued_count++;
	mpqueued_total++;
}
/**************************************************************************/
void queue_print(int seconds, char_data *ch, const char *text)
{
	mpqueue_type *node=new mpqueue_type(MQT_PRINT, seconds, ch, text);
	int hash=(int)node->when%MPQUEUE_HASH_SIZE;

	// queue it into the hash table buckets
	node->next=mpqueue_hash_table[hash];
	mpqueue_hash_table[hash]=node;

	// statistics stuff
	mpqueued_count++;
	mpqueued_total++;
}
/**************************************************************************/
