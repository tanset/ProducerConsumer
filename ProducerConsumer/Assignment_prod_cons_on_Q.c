/*
 * =====================================================================================
 *
 *       Filename:  Assignment.c
 *
 *    Description: Assignment on Producer And Consumer 
 *
 *        Version:  1.0
 *        Created:  01/29/2021 10:27:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ABHISHEK SAGAR (), sachinites@gmail.com
 *   Organization:  Juniper Networks
 * 
 *    Modified by:  Howie Chang
 *    Modified on:  2025/05/27
 *
 * =====================================================================================
 */

#if 0

Problem Statement :
====================
Write a program which launches 4 threads - 2 consumer threads and 2 producer threads. Threads
are created in JOINABLE Mode.
All 4 threads act on a shared resource - A Queue of integers. Producer threads produce
a random integer and add it to Queue, Consumer threads remove an integer from the Queue.
Maximum size of the Queue is 5.

Following are the constraints applied :

1. When producer threads produce an element and add it to the Queue, it does not release the Queue
untill the Queue is full i.e producer thread release the Queue only when it is full

2. When consumer threads consume an element from the Queue, it consumes the entire Queue and
do not release it until the Queue is empty.

3. Consumer Signals the Producers when Queue is Exhausted, Producers Signals the Consumers when Queue	
becomes full

Guidelines :
Use as many printfs as possible, so you can debug the program easily

Conmpile and Run :
gcc -g -c Queue.c -o Queue.o
#include <stdint.h> // 為了使用 intptr_t

...

int i;
while (!is_queue_full(Q)) {
    i = new_int();
    printf("%s produces new integer %d\n", th_name, i);
    enqueue(Q, (void*)(intptr_t)i); // 修正轉型
    printf("%s pushed %d, Queue size = %d\n", th_name, i, Q->count);
}

...

int i;
while (!is_queue_empty(Q)) {
    i = (int)(intptr_t)deque(Q); // 修正轉型
    printf("%s consumes %d, Queue size = %d\n", th_name, i, Q->count);
}
gcc -g Assignment_prod_cons_on_Q.o Queue.o -o exe -lpthread
 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h> // 為了使用 intptr_t
#include "Queue.h"

static int new_int() {

	static int a = 0;
	a++;
	return a;
}

struct Queue_t *Q;

static const char *prod1 = "TP1";
static const char *prod2 = "TP2";
static const char *cons1 = "TC1";
static const char *cons2 = "TC2";

static void *
prod_fn(void *arg) {

	char *th_name = (char *)arg;
	printf("%s waiting to lock the Queue\n", th_name);
	pthread_mutex_lock(&Q->q_mutex);
	printf("%s locks the Queue\n", th_name);

	while(is_queue_full(Q)){
		printf("%s blocks itself, Q is full\n", th_name);
		pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
		printf("%s wake up, checking the Queue status again\n", th_name);
	}

	assert(is_queue_empty(Q));

	int i;
	while(!is_queue_full(Q)){
		i = new_int();
		printf("%s produces new integer %d\n", th_name, i);
		enqueue(Q, (void*)(intptr_t)i);
		printf("%s pushed %d, Queue size = %d\n", th_name, i, Q->count);
	}

	printf("%s Filled up the Queue, signalling and releasing lock\n", th_name);
	pthread_cond_broadcast(&Q->q_cv);

	pthread_mutex_unlock(&Q->q_mutex);
	printf("%s finish and exit\n", th_name);
	return NULL;
	/* **************** 
	 * Join Telegram grp for QnA, Career Guidance,
	 * Dicount/Free Courses, Career Discussions etc.
 	 * Grp ID : telecsepracticals
 	 */

	/* Code Producer Logic here */
}

static void *
cons_fn(void *arg) {

	char *th_name = (char *)arg;
	
	/* **************** 
	 * Join Telegram grp for QnA, Career Guidance,
	 * Dicount/Free Courses, Career Discussions etc.
 	 * Grp ID : telecsepracticals
 	 */

	/* Code Consumer Logic here */
	printf("%s ready to lock the Queue\n", th_name);
	pthread_mutex_lock(&Q->q_mutex);
	printf("%s locks the Queue\n", th_name);

	while(is_queue_empty(Q)){
		printf("%s blocks itself, Q is empty\n", th_name);
		pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
		printf("%s wakes up, checking the Queue status again\n", th_name);
	}

	assert(is_queue_full(Q));

	int i;
	while(!is_queue_empty(Q)){
		i = (int)(intptr_t)deque(Q);
		printf("%s consumes %d, Queue size = %d\n",
			th_name, i, Q->count);
	}
	printf("%s Drains the entire Queue, sending signal to Blocking Threads\n",
		th_name);
	pthread_cond_broadcast(&Q->q_cv);

	printf("%s releasing lock\n", th_name);
	pthread_mutex_unlock(&Q->q_mutex);
	printf("%s finish and exit\n", th_name);
	return NULL;
}

int
main(int argc, char **argv) {

	/* initialize the Queue and its Mutex + CV */
	Q = initQ();
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_t prod_th1, prod_th2; /*  Two producer threads */
	pthread_t cons_th1, cons_th2; /*  Two consumer threads */

	pthread_create(
				&prod_th1, &attr, prod_fn, (void *)prod1);
	
	pthread_create(
				&prod_th2, &attr, prod_fn, (void *)prod2);
	
	pthread_create(
				&cons_th1, &attr, cons_fn, (void *)cons1);
	
	pthread_create(
				&cons_th2, &attr, cons_fn, (void *)cons2);

	pthread_join(prod_th1, 0);
	pthread_join(prod_th2, 0);
	pthread_join(cons_th1, 0);
	pthread_join(cons_th2, 0);

	printf("Program Finished\n");
	pthread_exit(0);
	return 0;
}

