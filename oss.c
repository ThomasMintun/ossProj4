#include "oss.h"

void ossClock();//function prototype
void sigint(int);//function prototype

int main(int argc, char* argv[]){
    int clockStop = 0;      // flag for stopping OS clock
    time_t t;               // for init random
    int forkMax = 0;

    srand((unsigned) time(&t));// init clock the clock
    signal(SIGINT, sigint);//signal handling config - ctrl-c
    sharedMemoryConfig();// configure shared memory

    //clock
    while(clockStop == 0){
        //increments clock and adjusts sec:nanos
        ossClock();

        //fork a child
        if(forkMax == 0){
            if ((PCBshmPtr->pidHolder = fork()) == 0)
                execl("./user", "user", NULL);
            forkMax++;
        }
    }
    // clean shared memory
    shmdt(sysClockshmPtr);
    return 0;
}

// signal handles ctrl-c
void sigint(int a){
    printf("^C caught\n");
    // clean shared memory
    shmdt(sysClockshmPtr);
    shmctl(sysClockshmid, IPC_RMID, NULL);
    shmdt(PCBshmPtr);
    shmctl(PCBshmid, IPC_RMID, NULL);
    exit(0);
}

//increments the clock random value and adjust sec:nanos
void ossClock(){
    int clockIncrement;     // random number to increment time
    int rollover;           // rollover nanos

    clockIncrement = (unsigned int) (rand() % 10000 + 1);//move this to 10,000 when 

    if ((sysClockshmPtr->nanoseconds + clockIncrement) > 999999999){
        rollover = (sysClockshmPtr->nanoseconds + clockIncrement) - 999999999;
        sysClockshmPtr->seconds += 1;
        sysClockshmPtr->nanoseconds = rollover;
    } else {
        sysClockshmPtr->nanoseconds += clockIncrement;
    }

    printf("%d\n", sysClockshmPtr->seconds);
    printf("%d\n", sysClockshmPtr->nanoseconds);

    //sleep(1);
}