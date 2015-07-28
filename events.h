#ifndef EVENTS_H
#define EVENTS_H

typedef struct Event_S {
	int nextInvoke;
	void (*call)(unsigned,unsigned);

	unsigned from;
	unsigned to;
	struct Event_S *next;

} Event;

void dumpEvent(Event * e); //for debugging

//Globals
extern Event *listEventHead;  //Head of the list with the events
extern Event *currentEvent;   //In the event loop this will have the current Context
extern Event *pendingEvents;  //Head of the list of events pending for registration

extern void (*tickCallback)(); //Function called after finishing every tick

/*
* Initialises the event handler
* Takes as argument a callback to a function that may be called 
* at the end of each tick
*/
void initEvent(void (*callback)());

/*
* Invokes the event loop. Does not return
* The delay is managed by the callback
*/
void eventLoop();


/*
* Performs all actions for one tick. 
* Can be used for a timer/interrupt driven schedule 
*/
void eventLoopInterrupt();

/*
* Registers an event in the handler. The arguments from and to describe the intervall the callback
* should work on. The callback get these as argument in the same order. The argument invoke
* describes in how many ticks the event is to be called.
*
* If the event is overlapping with an event that is already registered the event is removed.
* Registration is done at the end of each tick in the order the events were added.
*/
void registerEvent(void (*call)(unsigned,unsigned), unsigned from, unsigned to, unsigned invoke);

/*
* This function may be called inside an event callback to reschedule the current event.
*/
void recallEvent(unsigned ticks);

#endif
