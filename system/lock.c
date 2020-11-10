#include <xinu.h>

syscall initlock(lock_t *l);
syscall lock(lock_t *l);
syscall unlock(lock_t *l);

syscall setpark();
syscall park();
syscall unpark(pid32 p);

static uint32 lockFlag=0;

syscall setpark(){
    lockFlag=1;
    return OK;
}

syscall park(){
    intmask	mask;			     /* Saved interrupt mask		*/
    struct	procent *prptr;		/* Ptr to process's table entry	*/

    mask = disable();
    if(lockFlag==1){
        lockFlag=0;
        prptr = &proctab[currpid];
        prptr->prstate=PR_WAIT;
        restore(mask);
        resched();
    }
    restore(mask);
    return OK;
}

syscall unpark(pid32 p){
    intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
    if(lockFlag==0){
        restore(mask);
        ready(p);
    }
    restore(mask);
    return OK;
}

syscall initlock(lock_t *l){
    l->flag=0;
    l->guard=0;
    l->q=newqueue(); 
    return OK;
}

syscall lock(lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //acquire guard by spinning
    }
    if(l->flag==0){
        l->flag=1; //acquire lock
        l->guard=0;
    }
    else{
        enqueue(currpid,l->q);
        setpark();
        l->guard=0;
        park();
    }
    return OK;
}

syscall unlock(lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        ; //acquire guard by spinning
    }
    if(isempty(l->q)){
        l->flag=0; //free lock,no thread to acquire it
    }
    else{
        pid32 p=dequeue(l->q);
        unpark(p);
    }
    l->guard=0;
    return OK;
}
