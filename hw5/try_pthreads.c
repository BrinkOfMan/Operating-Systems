#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>


#define NUM_RECT 10000000 

pthread_mutex_t gLock; // Mutual exclusion object for accessing critical points


void *threadPid(void *pArg){ // Thread funtion: prints thread pid
    
    printf("I am the PTHREAD. My pid is %d\n", getpid());
    // Since threads are not new processes, all should have the same pid

    exit;
} 

int main(int argc, char **argv) {
    pthread_t tHandle; 
    int tData;

    printf("I am the MAIN. My pid is %d\n", getpid()); 

    pthread_mutex_init(&gLock, NULL);

    if(
        pthread_create(&tHandle,// Returned thread handle 
        NULL,                   // Thread Attributes
        threadPid,              // Thread function 
        (void*)&tData)       // Data for Area()
            != 0){ // pthread_create returns 0 on success
        printf("ERROR: pthread not created.\n");
    }
    else{ // Successful pthread creation
        printf("I am the MAIN, and I successfully launched a pthread.\n");
    }      
     

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

a) Do the main() and the pthread share the same process id on our system?

They do, and this makes sense, as a thread is not a separate process, rather a thread of execution
within a process. So, both the main() and pthread should have the same pid.

b) Do the pthread message and the middle message printed by main() (about launching the
pthread) always appear in the same order? Are they sometimes mixed in any way?

After running 25 trials: It appears the order is the same for reach trial. I think this would make sense,
as the pthread_join() should act as a kind of wait on main for the pthread to finish its work before continuing.

#################################################
*/