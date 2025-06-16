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

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h> // for intptr_t casting
#include "Queue.h"

// Generate new integer values for the producer
static int new_int() {
    static int a = 0;
    a++;
    return a;
}

struct Queue_t *Q;

// Thread name constants for producers and consumers
static const char *prod1 = "TP1";
static const char *prod2 = "TP2";
static const char *cons1 = "TC1";
static const char *cons2 = "TC2";

// Producer thread function
static void *
prod_fn(void *arg) {
    char *th_name = (char *)arg;
    printf("%s waiting to lock the Queue\n", th_name);

    // Lock the queue mutex before accessing shared resource
    pthread_mutex_lock(&Q->q_mutex);
    printf("%s locks the Queue\n", th_name);

    // If the queue is full, wait for signal from consumers
    while(is_queue_full(Q)){
        printf("%s blocks itself, Q is full\n", th_name);
        pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
        printf("%s wake up, checking the Queue status again\n", th_name);
    }

    // Assert the queue is empty before producing (per assignment logic)
    assert(is_queue_empty(Q));

    int i;
    // Produce and enqueue integers until the queue is full
    while(!is_queue_full(Q)){
        i = new_int();
        printf("%s produces new integer %d\n", th_name, i);
        enqueue(Q, (void*)(intptr_t)i); // Cast int to pointer type
        printf("%s pushed %d, Queue size = %d\n", th_name, i, Q->count);
    }

    // Once queue is full, notify waiting consumers and release the lock
    printf("%s Filled up the Queue, signalling and releasing lock\n", th_name);
    pthread_cond_broadcast(&Q->q_cv);

    pthread_mutex_unlock(&Q->q_mutex);
    printf("%s finish and exit\n", th_name);
    return NULL;
}

// Consumer thread function
static void *
cons_fn(void *arg) {
    char *th_name = (char *)arg;
    
    printf("%s ready to lock the Queue\n", th_name);

    // Lock the queue mutex before accessing shared resource
    pthread_mutex_lock(&Q->q_mutex);
    printf("%s locks the Queue\n", th_name);

    // If the queue is empty, wait for signal from producers
    while(is_queue_empty(Q)){
        printf("%s blocks itself, Q is empty\n", th_name);
        pthread_cond_wait(&Q->q_cv, &Q->q_mutex);
        printf("%s wakes up, checking the Queue status again\n", th_name);
    }

    // Assert the queue is full before consuming (per assignment logic)
    assert(is_queue_full(Q));

    int i;
    // Consume and dequeue integers until the queue is empty
    while(!is_queue_empty(Q)){
        i = (int)(intptr_t)deque(Q); // Cast pointer back to int
        printf("%s consumes %d, Queue size = %d\n",
            th_name, i, Q->count);
    }
    // Notify waiting producers that the queue has been emptied
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

    // Initialize the queue and its mutex/condition variable
    Q = initQ();
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t prod_th1, prod_th2; // Two producer threads
    pthread_t cons_th1, cons_th2; // Two consumer threads

    // Create producer and consumer threads
    pthread_create(
                &prod_th1, &attr, prod_fn, (void *)prod1);
    pthread_create(
                &prod_th2, &attr, prod_fn, (void *)prod2);
    pthread_create(
                &cons_th1, &attr, cons_fn, (void *)cons1);
    pthread_create(
                &cons_th2, &attr, cons_fn, (void *)cons2);

    // Wait for all threads to finish
    pthread_join(prod_th1, 0);
    pthread_join(prod_th2, 0);
    pthread_join(cons_th1, 0);
    pthread_join(cons_th2, 0);

    printf("Program Finished\n");
    pthread_exit(0);
    return 0;
}
