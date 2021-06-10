#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>


#define NUM_RECT 10000000 

pthread_mutex_t gLock; // Mutual exclusion object for accessing critical points

void *threadPid(void *pArg){ // Thread funtion: prints thread pid
    int *iptr = (int *)pArg;
    printf("I am the PTHREAD. My pid is %d\n", getpid());
    // Since threads are not new processes, all should have the same pid

    printf("Adding 1 to val.\n");
    *iptr += 1;
    printf("Val is now %d\n", *iptr);


    exit;
} 

int main(int argc, char **argv) {
    pthread_t tHandle; 
    // int tData; no longer in use
    int val = 10;

    printf("Val has been initialized with value %d\n", val);

    printf("I am the MAIN. My pid is %d\n", getpid()); 

    pthread_mutex_init(&gLock, NULL);

    if(
        pthread_create(&tHandle,// Returned thread handle 
        NULL,                   // Thread Attributes
        threadPid,              // Thread function 
        &val)       // Data for threadPid
            != 0){ // pthread_create returns 0 on success
        printf("ERROR: pthread not created.\n");
    }
    else{ // Successful pthread creation
        printf("I am the MAIN, and I successfully launched a pthread.\n");
    }      
     

    printf("Adding 2 to val.\n");
    val += 2;
    printf("Val is now %d\n", val);


    // Pause threads until pthread has finished its work
    if(pthread_join(tHandle, NULL) != 0){ // pthread_join returns 0 on success
        printf("ERROR: pthread not joined.\n"); 
    }
    else{
        printf("I am the MAIN, and the pthread has finished.\n");
    }
    

    pthread_mutex_destroy(&gLock); 
    exit(0);
}

/*
#################   Questions   #################

a) Does this program consistently return the value 13? If not, explain, why.

Running 25 tests: Sometimes, it seems the final printf shows val as 12 AFTER showing val as equal to 13.

This is likely due to a race condition, in that the final return value from the thread is ignored by main when adding 2 to val.

#################################################
*/