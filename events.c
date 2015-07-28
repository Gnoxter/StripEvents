
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "events.h"

Event *listEventHead = NULL;
Event *currentEvent = NULL;
Event *pendingEvents = NULL;
void (*tickCallback)() = NULL;


void initEvent(void (*callback)()) {
	tickCallback = callback;
}

void dumpEvent(Event * e) {
	printf("%p: Invoke: %i, %i - %i\n", e, e->nextInvoke, e->from, e->to);
}

void registerEvent(void (*call)(unsigned,unsigned), unsigned from, unsigned to, unsigned invoke) {
	Event *e = malloc(sizeof(Event));
	e->from = from;
	e->to = to;
	e->call = call;
	e->nextInvoke = invoke;
	e->next = NULL;

	Event *currentNode = pendingEvents;
	if(pendingEvents == NULL) {
		pendingEvents = e;
		return;
	}

	while(currentNode->next != NULL) {
		currentNode = currentNode->next;
	}

	currentNode->next = e;

}


void insertEvent(Event *e) {
	Event *currentNode, *linkIn, *tmp;
	//1st Special Case
	if (listEventHead == NULL) {
		listEventHead = e;
		return;
	}

	//2nd Special Case
	if(e->to < listEventHead->from) {
		e->next = listEventHead;
		listEventHead = e;
		return;
	}

	currentNode = listEventHead;

	/*
	* First step: Find the link in point for the event by 
	* searching the first occurrence where 
	*			e->from > currentNode->to
	*/

	while(currentNode->next != NULL) {

		if(e->from > currentNode->to) 
			break;

		currentNode = currentNode->next;
	}
	
	linkIn = currentNode;
	currentNode = currentNode->next;
	linkIn->next = e;	

	if(currentNode == NULL) {
		return;
	}


	/*
	* Second Step: Delete all following events
	* until e->to < currentNode->from
	*/

	do {
		if(e->to < currentNode->from)
			break;

		tmp = currentNode;
		currentNode = currentNode->next;
		free(tmp);
	} while(currentNode != NULL); 

	//Append the remaining events
	if(currentNode != NULL)	
		e->next = currentNode;	
}

void processPendingEvents() {
	Event *currentNode = pendingEvents, *tmp;
	while(currentNode != NULL) {
		tmp = currentNode;
		currentNode = currentNode->next;
		tmp->next = NULL;
		insertEvent(tmp);
	}

	pendingEvents = NULL;
}


void recallEvent(unsigned ticks) {
	currentEvent->nextInvoke = ticks;
}


void eventLoopInterrupt() {
	Event *previousEvent, *tmp;
	currentEvent = listEventHead;
	previousEvent = NULL;

	while(currentEvent != NULL) {
		if(currentEvent->nextInvoke == 0) {
			currentEvent->call(currentEvent->from, currentEvent->to);
			//Event got renewed
			if(currentEvent->nextInvoke != 0) 
				continue;

			//First Element of the list
			if(previousEvent == NULL) {
				tmp = currentEvent;
				currentEvent = currentEvent->next;
				listEventHead = currentEvent;
				free(tmp);
			} else {
				tmp = currentEvent;
				previousEvent->next = currentEvent->next;
				currentEvent = currentEvent->next;
				free(tmp);
			}
		
		} else {
			currentEvent->nextInvoke--;
			//Next event
			previousEvent = currentEvent;
			currentEvent = currentEvent->next;
		}
	}
	
	processPendingEvents();
	if(tickCallback != NULL)
		tickCallback();
}

void eventLoop() {
 head: //You spin my head right round, right round
	eventLoopInterrupt();
 goto head; //when you go down, when you go down
}
