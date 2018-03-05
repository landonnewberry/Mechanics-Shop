#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "sll.h"

queue* newQueue(void (*d)(FILE*, void*))
{
    queue* theQueue = (queue*)malloc(sizeof(queue));
    theQueue->queueSLL = newSLL(d);
    return theQueue;
}

void enqueue(queue* items, void* value)
{
    insertSLL(items->queueSLL, sizeSLL(items->queueSLL), value);
}

void* dequeue(queue* items)
{
    return removeSLL(items->queueSLL, 0);
}

void* peekQueue(queue* items)
{
    return getSLL(items->queueSLL, 0);
}

int sizeQueue(queue* items)
{
    return sizeSLL(items->queueSLL);
}

void displayQueue(FILE* fp, queue* items)
{
    displaySLL(fp, items->queueSLL);
}