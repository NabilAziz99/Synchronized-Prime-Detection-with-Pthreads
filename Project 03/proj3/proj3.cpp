#include <pthread.h>
#include <ctime>
#include <stdlib.h>
#include "buffer.h"
#include<unistd.h>
#include<iostream>
#include<cstring>
using namespace std;

int threadTime;
char* showStats;

void printBuffer() {
    cout<< "Buffer Occupied : " << count <<endl;
    cout<< "Buffer : ";
    for(int i = 0 ; i < BUFFER_SIZE ; i++){
        cout<< buffer[i] << " , ";
    }
    cout<< endl;
}


bool isPrime(int n) {
    if (n <= 1)
        return false;

    for (int i = 2; i < n; i++)
        if (n % i == 0)
            return false;
 
    return true;
}


void* producer(void* arg) {
    thread_local unsigned int seed = time(NULL);
    buffer_item item = rand() % 256;

    // Sleep if Buffer is full
    sem_trywait(&empty);
    // Exclusive Access of Buffer
    sem_wait(&countMutex);
    sleep(threadTime);
    // Enter item
    buffer_insert_item(item);
    cout <<"\nProducer :  " << pthread_self() << " writes : " << item << endl;
    if(!strcmp(showStats , "yes")) 
        printBuffer();
    // leave Exclusive access of the Buffer    
    sem_post(&countMutex);
    // Wakeup if first item in buffer
    sem_post(&full);
    
    pthread_exit(0); 
}


void* consumer(void* arg) {
    buffer_item item;

    // Sleep if buffer empty
    sem_trywait(&full);
    // Get Exlcusive Access of Buffer
    sem_wait(&countMutex);
    sleep(threadTime);
    // Enter Item
    buffer_remove_item(item);
    if(isPrime(item))
        cout <<"\nConsumer :  " << pthread_self() << " reads : " << item  << " * * * PRIME * * *" << endl;
    else
            cout <<"\nConsumer :  " << pthread_self() << " reads : " << item << endl;   
    if(!strcmp(showStats , "yes"))        
        printBuffer();
    
    // Leave Exclusive access of Buffer
    sem_post(&countMutex);
    // Wakeup of buffer has space
    sem_post(&empty);
    

    pthread_exit(0);
}

int main(int argc , char* argv[]) {
    
    if (argc != 6) {
        cout << "Provide correct number of arguments , Provided = " << argc - 1 << " , Required = 5 \n.";
        return -1;
    }

    int numProducers = atoi(argv[1]) , numConsumers = atoi(argv[2]) , mainTime = atoi(argv[3]);
    threadTime = atoi(argv[4]);
    showStats = argv[5];

    cout << "Number of Producers : " << numProducers <<endl;
    cout << "Number of Consumers : " << numConsumers <<endl;
    cout << "Sleep time for Main Thread : " << mainTime <<endl;
    cout << "Sleep time for Consumer/Producer Threads : " << threadTime <<endl;
    cout << "Show Stats : " << showStats <<endl;

    buffer = new int[BUFFER_SIZE];
    intializeBuffer();
    pthread_t producers[numProducers];
    pthread_t consumers[numConsumers];

    int start_s = clock();
    cout<<"\nStarting Thread...\n";
    printBuffer();
    for(int i = 0; i < numProducers; i++) {
        if( pthread_create(&producers[i] , NULL , producer , NULL) ) {
            cout << "Creation of producer thread failed \n";
            return -1;
        }
    }
     
    for(int i = 0; i < numConsumers; i++) {
        if( pthread_create(&consumers[i] , NULL , consumer , NULL) ) {
            cout << "Creation of consumer thread failed \n";
            return -1;
        }
    } 

    sleep(mainTime);
    // cout<< "Threads Joining...\n";

    // main thread waits for producers to join
    for(int i = 0; i < numProducers; i++) {
        if( pthread_join(producers[i] , NULL) ) {
            cout << "Producer Join failed \n";
            return -1;
        }
    }
    
    // Main thread waits for consumers to join
    for(int i = 0; i < numConsumers; i++) {
        if( pthread_join(consumers[i], NULL) ) {
            cout << "Consumer Join failed \n";
            return -1;
        }
    }

    cout << "\n\t\t Simulation Complete \n";
    int stop_s = clock();
    cerr << "Time Taken : " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 <<"ms"<< endl;
    cout << "Maximum Thread Sleep Time : " << threadTime << endl;
    cout << "Buffer Size : " << BUFFER_SIZE << endl;
    
    cout<<"\n\t\tRemaining Buffer : \n";
    printBuffer();

    cout<<"Number of Times Buffer was Full : " << bufferFull <<endl;
    cout<<"Number of Times Buffer was Empty : " << bufferEmpty <<endl;

    delete buffer;
    return 0;
}
