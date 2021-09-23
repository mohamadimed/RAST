#include <stdio.h>
#include <stdlib.h>
#include "net/mac/tsch/tsch-circular-queue.h"




// Utility function to initialize queue
struct queue* newQueue(uint8_t size)
{ 
	struct queue *pt = NULL;
	pt = (struct queue*)malloc(sizeof(struct queue));
	
	pt->items = (ebr_informations*)malloc(size * sizeof(ebr_informations));
	pt->maxsize = size;
	pt->front = 1;
	pt->rear = -1;
	pt->size = 0;

	return pt;
}

// Utility function to return the size of the queue
uint8_t size(struct queue *pt)
{
	return pt->size;
}

// Utility function to check if the queue is empty or not
uint8_t isEmpty(struct queue *pt)
{	
	if (pt->size==0)
	return 1;
	else 
	return 0;
}

// Utility function to return front element in queue
uint8_t get_communication_channel(struct queue *pt)
{     
	
/*
	if (isEmpty(pt))
	{
		printf("UnderFlow\nProgram Terminated\n");
		return p;
	}*/ //printf("-- COM %u\n",pt->items[pt->front].communication_channel);

	return pt->items[pt->front].communication_channel;
}

uint8_t get_wakeup_time(struct queue *pt)
{     
	
/*
	if (isEmpty(pt))
	{
		printf("UnderFlow\nProgram Terminated\n");
		return p;
	}*/

	return pt->items[pt->front].wakeup_time;
}

// Utility function to add an element x in the queue
void enqueue(struct queue *pt, uint8_t x,uint8_t y)
{
	/*if (size(pt) == pt->maxsize)
	{
		printf("OverFlow\nProgram Terminated\n");
		return ;
	}*/

	//printf("Inserting %d\t", x);

	pt->rear = (pt->rear + 1) % pt->maxsize;	// circular queue
	pt->items[pt->rear].communication_channel = x; //printf("---ENQUEUE CH %u\n",pt->items[pt->rear].communication_channel);
	pt->items[pt->rear].wakeup_time = y; //printf("---Ti %u\n",pt->items[pt->rear].wakeup_time*100);
	pt->size++;									

	//printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

// Utility function to remove element from the queue
void dequeue(struct queue *pt)
{
	if (isEmpty(pt)) // front == rear
	{
		//printf("UnderFlow\nProgram Terminated\n");
		return ;
	}

	//printf("--Removing  \n");

	pt->front = (pt->front + 1) % pt->maxsize;	// circular queue
	pt->size--;

	//printf("front = %d, rear = %d\n", pt->front, pt->rear);
}

