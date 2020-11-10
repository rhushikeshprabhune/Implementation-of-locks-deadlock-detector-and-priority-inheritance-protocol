#include <xinu.h>

void run_for_ms(uint32 time){
    uint32 start = proctab[currpid].runtime;
    while (proctab[currpid].runtime - start < time)
        ;
}

al_lock_t activeLock1;
al_lock_t activeLock2;
al_lock_t activeLock3;

al_lock_t lock0;
al_lock_t lock1;


/* functions for part 1 */
process processA(al_lock_t *activeLock1, al_lock_t *activeLock2){
    uint32 i;
    for (i = 0; i < 5; i++)
    {
        al_lock(activeLock1);
        run_for_ms(1100);
        al_lock(activeLock2);
        run_for_ms(1000);
        al_unlock(activeLock2);
        al_unlock(activeLock1);
    }
    return OK;
}

process processB(al_lock_t *activeLock2, al_lock_t *activeLock3){
    uint32 i;
    for (i = 0; i < 5; i++)
    {
        al_lock(activeLock2);
        run_for_ms(1000);
        al_lock(activeLock3);
        run_for_ms(1000);
        al_unlock(activeLock3);
        al_unlock(activeLock2);
    }
    return OK;
}

process processC(al_lock_t *activeLock3, al_lock_t *activeLock1){
    uint32 i;
    for (i = 0; i < 5; i++)
    {
        al_lock(activeLock3);
        run_for_ms(1000);
        al_lock(activeLock1);
        run_for_ms(1000);
        al_unlock(activeLock1);
        al_unlock(activeLock3);
    }
    return OK;
}
/* functions for part 2 */
void processA_trylock(){
  top: al_lock(&lock0);
  kprintf("Lock 0 acquired by pid: %d\n", currpid);
  if(al_trylock(&lock1)==1){
    al_unlock(&lock0);
    goto top;
  }
  kprintf("Lock 1 acquired by pid: %d\n", currpid);
  run_for_ms(1100);
  al_unlock(&lock1);
  kprintf("Lock 1 released by pid: %d\n", currpid);
  al_unlock(&lock0);
  kprintf("Lock 0 released by pid: %d\n", currpid);
}

void processB_trylock(){
  top: al_lock(&lock1);
  kprintf("Lock 1 acquired by pid: %d\n", currpid);
  if(al_trylock(&lock0)==1){
    al_unlock(&lock1);
    goto top;
  }
  kprintf("Lock 0 acquired by pid: %d\n", currpid);
  run_for_ms(1000);
  al_unlock(&lock0);
  kprintf("Lock 0 released by pid: %d\n", currpid);
  al_unlock(&lock1);
  kprintf("Lock 1 released by pid: %d\n", currpid);
}

int main(){
    pid32 pid1, pid2, pid3;

    kprintf("\nPart 1: Deadlock Detection\n");
    al_initlock(&activeLock1);
    al_initlock(&activeLock2);
	al_initlock(&activeLock3);

    pid1 = create((void *) processA, INITSTK, 1, "processA", 2, &activeLock1,
            &activeLock2);
    pid2 = create((void *) processB, INITSTK, 1, "processB", 2, &activeLock2,
            &activeLock3);
	pid3 = create((void *) processC, INITSTK, 1, "processC", 2, &activeLock3,
            &activeLock1);
    resume(pid1);
    sleepms(500);
    resume(pid2);
	sleepms(500);
	resume(pid3);
	sleep(4);
	kprintf("\nDeadlock Detected!\n");
	sleep(2);

	kprintf("\nPart 2: Deadlock Avoidance using trylock\n");

	al_initlock(&lock0);
  	al_initlock(&lock1);
 
  	resume(create(processA_trylock, INITSTK, 7, "processA_trylock", 0, NULL));
  	resume(create(processB_trylock, INITSTK, 7, "processB_trylock", 0, NULL));

  	receive();
  	receive();
 	kprintf("\nNo Deadlock. TEST PASSED!\n");

    return 0;
}