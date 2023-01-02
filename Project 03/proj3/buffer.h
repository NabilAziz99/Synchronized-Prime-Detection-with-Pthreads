#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>

#define BUFFER_SIZE 5
typedef int buffer_item;
buffer_item* buffer;
unsigned count = 0;

int front = -1 , rear = -1;
int bufferFull= 0 , bufferEmpty = 0; 

sem_t empty;
sem_t full;
sem_t countMutex;


void intializeBuffer() {

    sem_init(&empty , 0 , BUFFER_SIZE);
    sem_init(&full , 0 , 0);
    sem_init(&countMutex , 0 , 1);

    for(int i = 0 ; i < BUFFER_SIZE ; i++)
        buffer[i] = -1; 
}

bool buffer_insert_item(buffer_item item) {

    if((front == 0 && rear == BUFFER_SIZE - 1) || (rear == (front-1) % (BUFFER_SIZE-1))) {
        bufferFull++;
        return false;
    }
    else if (front == -1)  {
        front = rear = 0;
        buffer[rear] = item;
    }
    else if (rear == BUFFER_SIZE - 1 && front != 0) {
        rear = 0;
        buffer[rear] = item;
    }
    else {
        rear++;
        buffer[rear] = item;
    }

    count++;
    return true;
}

bool buffer_remove_item(buffer_item& item){
    if (front == -1) {
        bufferEmpty++;
        return false;
    }
    
    item = buffer[front];
    buffer[front] = -1;
    if (front == rear) {
        front = -1;
        rear = -1;
    }
    else if (front == BUFFER_SIZE - 1)
        front = 0;
    else
        front++;

    count--;
    return item;
}

#endif