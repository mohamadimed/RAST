#ifndef __TSCH_CIRCUALR_QUEUE_H
#define __TSCH_CIRCUALR_QUEUE_H

#undef CIRCULAR_QUEUE_SIZE
#define CIRCULAR_QUEUE_SIZE 5

/* This a structure to hold the channel used to receive an EB as a response to EBR and the time in which we will be on to receive EB as response to an EBR*/
typedef struct ebr_informations {

uint8_t communication_channel;
uint8_t wakeup_time;

} ebr_informations ;


// Data structure for queue
struct queue
{
	ebr_informations *items;		// array to store queue elements
	uint8_t maxsize;		// maximum capacity of the queue
	uint8_t front;		// front points to front element in the queue (if any)
	uint8_t rear;		// rear points to last element in the queue
	uint8_t size;		// current capacity of the queue
};
/*Prototype of functions*/

/* function to initialize queue */
struct queue* newQueue(uint8_t size);

/* function to return the size of the queue*/
uint8_t size(struct queue *pt);

/* function to check if the queue is empty or not */
uint8_t isEmpty(struct queue *pt);

/* Utility function to return front element in queue */
uint8_t get_communication_channel(struct queue *pt);
uint8_t get_wakeup_time(struct queue *pt);
/* Utility function to add an element x in the queue */
void enqueue(struct queue *pt, uint8_t x, uint8_t y);

/* Utility function to remove element from the queue */
void dequeue(struct queue *pt);


#endif
