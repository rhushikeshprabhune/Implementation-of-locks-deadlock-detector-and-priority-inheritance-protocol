/*  main.c  - main */

#include <xinu.h>
#include <stdlib.h>

void run_for_ms(uint32 time){
    uint32 start = proctab[currpid].runtime;
    while (proctab[currpid].runtime-start < time);
}

pi_lock_t lock1;
pi_lock_t lock2;

void processA(){
    pi_lock(&lock1);
    run_for_ms(12000);
    pi_unlock(&lock1);
}

void processB(){ 
    sleepms(200);
    pi_lock(&lock2);
    run_for_ms(400);
    pi_lock(&lock1);
    run_for_ms(2000);
    pi_unlock(&lock1);
    pi_unlock(&lock2);
}

void processC() {
    sleepms(5000);
    pi_lock(&lock2);
    run_for_ms(6000);
    pi_unlock(&lock2);
}

process main(void)
{
    pi_initlock(&lock1);
    pi_initlock(&lock2);
    resume(create(processA, INITSTK, 5, "processA", 0, NULL));
    resume(create(processB, INITSTK, 9, "processB", 0, NULL));
    resume(create(processC, INITSTK, 14, "processC", 0, NULL));
    receive();
    receive();
    receive();
    sync_printf("\nTEST PASSED!\n");
    return OK;
}