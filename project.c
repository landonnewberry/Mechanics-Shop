/*
 *	Landon Newberry
 * 	ltnewberry@crimson.ua.edu
 *	Project 1
 *	CS426 - Anderson
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "datastructures/queue.h"

#define FILE_NAME   "jobcard"
#define NAME_LENGTH 50

/* macros defined for simplicity :-) */
#define LOCK(x)     pthread_mutex_lock(x)
#define UNLOCK(x)   pthread_mutex_unlock(x)
#define SIGNAL(x)   pthread_cond_signal(x)
#define WAIT(x,y)   pthread_cond_wait(x,y)


typedef struct {
	pthread_t thread;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int ready;
	char* name;
	int mechanic_time;
	int oil_time;
	int offset;
} customer;

struct {
	pthread_t thread;
	queue* lot;				// "parking lot" queue
	int lot_size;			// maximum size for the parking lot
	pthread_mutex_t lock;	
	pthread_cond_t cond;
	int sleep;				// 1 or 0: is mechanic asleep?
	int busy;				// 1 or 0: is mechanic currently servicing customer?
	customer* previous;		// pointer to the last customer served by mechanic
} mechanic;

struct {
	pthread_t thread;
	queue* lot;				// "parking lot" queue
	pthread_mutex_t lock;
	pthread_cond_t cond;
	int sleep;				// 1 or 0: is oil tech asleep?
	int busy;				// 1 or 0: is oil tech currently servicing customer?
	customer* previous;		// pointer to the last customer served by oil tech
} oil_tech;



/*
 *	Initialize all of the components of the mechanic struct
 */
void mechanic_init(void) {
	pthread_mutex_init(&mechanic.lock, NULL);
	pthread_cond_init(&mechanic.cond, NULL);
	mechanic.lot = newQueue(NULL);
	mechanic.sleep = 1;
	mechanic.busy = 0;
	mechanic.previous = NULL;
}


/*
 * This process acts as a symbolic mechanic. Always either servicing a customer 
 * or sleeping.
 */
void* mechanic_entry(void* varg) {
	for (;;) {

		LOCK(&mechanic.lock);
		if (sizeQueue(mechanic.lot) > 0 && !mechanic.busy) { // if lot not empty and mechanic not busy
			mechanic.busy = 1;
			customer* c = dequeue(mechanic.lot);
			if (mechanic.previous != NULL) printf("Customer %s - (MB2) is no longer waiting for mechanic; notified by %s\n", c->name, mechanic.previous->name);
			UNLOCK(&mechanic.lock);

			printf("Customer %s - (MC) is being serviced by the mechanic for %d second(s)\n", c->name, c->mechanic_time);
			sleep(c->mechanic_time);

			c->ready = 1;
			SIGNAL(&c->cond);	// tell customer that they are done being serviced

			LOCK(&mechanic.lock);
			// if there is a customer to notify next, c becomes mechanic.previous
			if (sizeQueue(mechanic.lot) > 0) mechanic.previous = c;
			// else store NULL as mechanic.previous so we know that the next customer was not referred
			else mechanic.previous = NULL;
			UNLOCK(&mechanic.lock);
		}
		else if (sizeQueue(mechanic.lot) == 0) {
			mechanic.sleep = 1;
			UNLOCK(&mechanic.lock);
		} else UNLOCK(&mechanic.lock);

		// if nothing to do, mechanic goes to sleep
		LOCK(&mechanic.lock);
		while (mechanic.sleep == 1) WAIT(&mechanic.cond, &mechanic.lock);
		UNLOCK(&mechanic.lock);

	}
	return NULL;
}






/*
 * Initialize all of the components of the oil tech struct
 */
void oil_tech_init(void) {
	pthread_mutex_init(&oil_tech.lock, NULL);
	pthread_cond_init(&oil_tech.cond, NULL);
	oil_tech.lot = newQueue(NULL);
	oil_tech.sleep = 1;
	oil_tech.busy = 0;
	oil_tech.previous = NULL;
}


/*
 * This process acts as a symbolic oil tech. Always servicing customers or
 * sleeping.
 */
void* oil_tech_entry(void* varg) {
	for (;;) {

		LOCK(&oil_tech.lock);
		if (sizeQueue(oil_tech.lot) > 0 && !oil_tech.busy) { // if lot not empty and mechanic not busy
			oil_tech.busy = 1;
			customer* c = dequeue(oil_tech.lot);
			if (oil_tech.previous != NULL) printf("Customer %s - (MBL2) is no longer waiting for oil change tech; signaled by %s\n", c->name, oil_tech.previous->name);
			UNLOCK(&oil_tech.lock);

			printf("Customer %s - (MCL) is being serviced by the oil change tech for %d second(s)\n", c->name, c->oil_time);
			sleep(c->oil_time);

			c->ready = 1;
			SIGNAL(&c->cond);	// tell customer that they are done being serviced

			LOCK(&oil_tech.lock);
			// if there is a customer to notify next, c becomes oil_tech.previous
			if (sizeQueue(oil_tech.lot) > 0) oil_tech.previous = c;
			// else store NULL as oil_tech.previous so we know that the next customer was not referred
			else oil_tech.previous = NULL;
			UNLOCK(&oil_tech.lock);
		}
		else if (sizeQueue(oil_tech.lot) == 0) {
			oil_tech.sleep = 1;
			UNLOCK(&oil_tech.lock);
		} else UNLOCK(&oil_tech.lock);

		// if nothing to do, oil tech goes to sleep
		LOCK(&oil_tech.lock);
		while (oil_tech.sleep == 1) WAIT(&oil_tech.cond, &oil_tech.lock);
		UNLOCK(&oil_tech.lock);

	}
	return NULL;
}


/* Initializes customer structs based on params read in from jobcard */
customer* newCustomer(char* name, int mechanic_time, int oil_time, int offset) {
	customer* c = (customer*)malloc(sizeof(customer));
	pthread_mutex_init(&c->lock, NULL);
	pthread_cond_init(&c->cond, NULL);
	c->ready = 0;
	c->name = name;
	c->mechanic_time = mechanic_time;
	c->oil_time = oil_time;
	c->offset = offset;
	return c;
}


/* Puts customer struct into the appropriate queue for either mechanic or oil tech */
void enter_lot(queue* lot, customer* c) {
	if (lot == mechanic.lot) {
		LOCK(&mechanic.lock);
		if (sizeQueue(mechanic.lot) > 0 || mechanic.busy) printf("Customer %s - (MB1) is waiting for mechanic\n", c->name);
		enqueue(mechanic.lot, c);
		if (mechanic.sleep == 1) {
			mechanic.sleep = 0;
			UNLOCK(&mechanic.lock);
			SIGNAL(&mechanic.cond);
		}
		else UNLOCK(&mechanic.lock);
	}
	else if (lot == oil_tech.lot) {
		LOCK(&oil_tech.lock);
		if (sizeQueue(oil_tech.lot) > 0 || oil_tech.busy) printf("Customer %s - (MBL1) is waiting for oil change tech\n", c->name);
		enqueue(oil_tech.lot, c);
		if (oil_tech.sleep == 1) {
			oil_tech.sleep = 0;
			UNLOCK(&oil_tech.lock);
			SIGNAL(&oil_tech.cond);
		}
		else UNLOCK(&oil_tech.lock);
	}
}





/*
 * This process provides the main functionality for customers.
 */
void* customer_entry(void* varg) {
	customer* c = varg;
	
	LOCK(&mechanic.lock);
	if (sizeQueue(mechanic.lot) >= mechanic.lot_size) { // if lot is already full
		UNLOCK(&mechanic.lock);
		printf("Customer %s - (MZ) leaves busy car maintenance shop\n", c->name);
		pthread_exit(NULL);
	}
	else {
		UNLOCK(&mechanic.lock);
		
		enter_lot(mechanic.lot, c);
		
		LOCK(&c->lock);
		while (!c->ready) WAIT(&c->cond, &c->lock);
		UNLOCK(&c->lock);

		LOCK(&mechanic.lock);
		if (sizeQueue(mechanic.lot) > 0) {
			customer* next_c = peekQueue(mechanic.lot);
			printf("Customer %s - (MC1) notifying customer %s that they are next for mechanic\n", c->name, next_c->name);
		}
		printf("Customer %s - (MD) is done with mechanic\n", c->name);
		mechanic.busy = 0;
		UNLOCK(&mechanic.lock);

		if (c->oil_time > 0) { // if they also need an oil change
			c->ready = 0;
			enter_lot(oil_tech.lot, c);
			LOCK(&c->lock);
			while (!c->ready) WAIT(&c->cond, &c->lock);
			UNLOCK(&c->lock);

			LOCK(&oil_tech.lock);
			if (sizeQueue(oil_tech.lot) > 0) {
				customer* next_c = peekQueue(oil_tech.lot);
				printf("Customer %s - (MCL1) notifying customer %s that they are next for oil change tech\n", c->name, next_c->name);
			}
			printf("Customer %s - (ML) is done with oil change tech\n", c->name);
			oil_tech.busy = 0;
			UNLOCK(&oil_tech.lock);
		}
		printf("Customer %s - (ME) is leaving shop\n", c->name);
	}
	pthread_exit(NULL);
	return NULL;
}



int main(void) {
	mechanic_init();
	oil_tech_init();

	/* READ INPUT */
	FILE* fp = fopen(FILE_NAME, "r");
	if (fp == NULL) exit(0);
	fscanf(fp, "%d", &mechanic.lot_size);
	while (fgetc(fp) != '\n');
	queue* q = newQueue(NULL);	// customer processing queue
	char* name = (char*)malloc(sizeof(NAME_LENGTH));
	int offset, mech_time, oil_time;
	while (4 == fscanf(fp, "%[^,],%d,%d,%d\n", name, &offset, &mech_time, &oil_time)) {
		customer* c = newCustomer(name, mech_time, oil_time, offset);
		enqueue(q, c);
		name = malloc(sizeof(char) * NAME_LENGTH);
	}
	fclose(fp);
	/* DONE READING INPUT */

	/* CREATE MECHANIC AND OIL TECH THREADS */
	pthread_create(&mechanic.thread, NULL, mechanic_entry, NULL);
	pthread_create(&oil_tech.thread, NULL, oil_tech_entry, NULL);
	/* DONE CREATING MECHANIC AND OIL TECH THREADS */

	/* CREATE EACH CUSTOMER THREAD IN ORDER */
	for (int prev = 0, i = sizeQueue(q); i--;) {
		customer* c = dequeue(q);
		sleep(c->offset - prev);
		pthread_create(&c->thread, NULL, customer_entry, c);
		prev = c->offset;
		enqueue(q, c);
		printf("Customer %s - (MA) arrival\n", c->name);
	}
	/* DONE CREATING EACH CUSTOMER THREAD IN ORDER */

	/* JOIN THREADS */
	while (sizeQueue(q)) {
		customer* c = dequeue(q);
		if (pthread_join(c->thread, NULL)) {
			printf("Error joining thread.\n");
			exit(1);
		}
	}
	/* DONE JOINING THREADS */

	// don't join mechanic and oil tech threads so the program
	// terminates after last customer... otherwise the mechanic and
	// oil tech threads would suspend execution of main indefinitely

	return 0;
}
