# Producer-Consumer Problem with Circular Queue (Multithreading in C)

This project demonstrates a classic **Producer-Consumer** problem using multithreading in C, with a custom circular queue implementation for thread-safe communication between producers and consumers.

## Features

- **4 Threads:** 2 producer threads and 2 consumer threads.
- **Thread Synchronization:** Uses mutex and condition variables for safe concurrent access.
- **Circular Queue:** Fixed size (default 5), implemented in C.
- **Blocking Behavior:**  
  - Producers fill the queue and only release the lock when the queue is full.  
  - Consumers drain the queue and only release the lock when the queue is empty.
- **Debug Output:** Extensive `printf` statements for step-by-step tracing.

## Problem Statement

- Producers generate random integers and add them to the shared queue.
- Consumers remove integers from the queue.
- Producers wait if the queue is full; consumers wait if the queue is empty.
- Proper signaling between producers and consumers using condition variables.

## Build & Run

```sh
gcc -g -c [Queue.c](http://_vscodecontentref_/0) -o [Queue.o](http://_vscodecontentref_/1)
gcc -g [Assignment_prod_cons_on_Q.c](http://_vscodecontentref_/2) [Queue.o](http://_vscodecontentref_/3) -o prod_cons -lpthread
./prod_cons
