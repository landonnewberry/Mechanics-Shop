#include <stdio.h>
#include "sll.h"

#ifndef __QUEUE_INCLUDED__
#define __QUEUE_INCLUDED__


typedef struct queue {
    sll* queueSLL;
} queue;


extern queue* newQueue(void (*d)(FILE*, void*));
extern void enqueue(queue* items, void* value);
extern void* dequeue(queue* items);
extern void* peekQueue(queue* items);
extern int sizeQueue(queue* items);
extern void displayQueue(FILE* fp, queue* items);


 #endif