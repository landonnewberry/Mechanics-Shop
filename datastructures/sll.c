#include <stdio.h>
#include <stdlib.h>
#include "sll.h"

sll* newSLL(void (*d)(FILE* fp, void* value))
{
    sll* theSLL = (sll*)malloc(sizeof(sll));
    if (theSLL == 0) printf("out of memory\n");
    theSLL->head = NULL;
    theSLL->tail = NULL;
    theSLL->size = 0;
    theSLL->display = d;
    return theSLL;
}

void insertSLL(sll* items, int index, void* value)
{
    if(items->size == 0)
    {
        sllnode* newNode = (sllnode*)malloc(sizeof(sllnode));
        newNode->value = value;
        newNode->next = NULL;
        items->head = newNode;
        items->tail = newNode;
        items->size = (int)items->size + 1;
        return;
    }
    else if (items->size == index)
    {
        sllnode* newNode = (sllnode*)malloc(sizeof(sllnode));
        newNode->value = value;
        newNode->next = NULL;
        items->tail->next = newNode;
        items->tail = newNode;
        items->size = (int)items->size + 1;
        return;
    }
    else
    {
        sllnode* newNode = (sllnode*)malloc(sizeof(sllnode));
        newNode->value = value;
        sllnode* n = items->head;
        sllnode* b = NULL;
        int i = 0;
        while(n)
        {
            if(i == index)
            {
                if(index == 0)
                {
                    newNode->next = items->head;
                    items->head = newNode;
                    items->size = (int)items->size + 1;
                    return;
                }
                else
                {
                    b->next = newNode;
                    newNode->next = n;
                    items->size = items->size + 1;
                    return;
                }
            }
            if(!n->next)
            {
                items->tail->next = newNode;
                newNode->next = NULL;
                items->tail = newNode;
                items->size = items->size + 1;
                return;
            }
            ++i;
            b = n;
            n = n->next;
        }
    }
}

void displaySLL(FILE* fp, sll* items)
{
    sllnode* p = items->head;
    fprintf(fp, "[");
    while(p)
    {
        if(p->next)
        {
            items->display(fp, p->value);
            fprintf(fp, ",");
        }
        else
        {
            items->display(fp, p->value);
        }
        p = p->next;
    }
    fprintf(fp, "]");
}

void* removeSLL(sll* items, int index)
{
    if(items->size == 1)
    {
        void* returnThis = items->head->value;
        items->head = NULL;
        items->tail = items->head;
        items->size = 0;
        return returnThis;
    }
    if(index == 0)
    {
        void* returnThis = items->head->value;
        items->head = items->head->next;
        items->size = items->size - 1;
        return returnThis;
    }
    else
    {
        int i = 0;
        sllnode* n = items->head;
        sllnode* b = NULL;
        while(n)
        {
            if((!n->next->next) && (index == i + 1))
            {
                void* returnThis = n->next->value;
                items->tail = n;
                n->next = NULL;
                items->size = items->size - 1;
                return returnThis;
            }
            else if(i == index)
            {
                void* returnThis = n->value;
                b->next = n->next;
                items->size = items-> size - 1;
                return returnThis;
            }
            b = n;
            n = n->next;
            ++i;
        }
    }
    void* returnThis = NULL;
    return returnThis;
}

void unionSLL(sll* recipient, sll* donor)
{
    if (recipient->head && donor->head)
    {
        recipient->tail->next = donor->head;
        recipient->tail = donor->tail;
        recipient->size = (int)recipient->size + donor->size;
        donor->head = NULL;
        donor->tail = NULL;
        donor->size = 0;
        return;
    }
    // donor is null 
    else if (recipient->head)
    {
        return;
    }
    // recipient is null 
    else if (donor->head)
    {
        recipient->head = donor->head;
        recipient->tail = donor->tail;
        recipient->size = donor->size;
        donor->head = NULL;
        donor->tail = NULL;
        donor->size = 0;
        return;
    }
    // both are null
    else
    {
        return;
    }
}

void* getSLL(sll* items, int index)
{
    int i = 0;
    sllnode* p = items->head;
    while(p)
    {
        if(i == index)
        {
            void* returnThis = p->value;
            return returnThis;
        }
        p = p->next;
        i++;
    }
    void* returnThis = NULL;
    return returnThis;
}

int sizeSLL(sll* items)
{
    return (int)items->size;
}