//in lock
//queue of current lock holds all processes waiting on the lock. iterate the queue
//get the pid's, get the thread priorities using these pid's, pick the highest priority,
//give this highest priority to currpid
//
#include <xinu.h>

syscall pi_initlock(pi_lock_t *l);
syscall pi_lock(pi_lock_t *l);
syscall pi_unlock(pi_lock_t *l);

syscall pi_setpark();
syscall pi_park();
syscall pi_unpark(pid32 p);

static uint32 lockFlag=0;
static uint32 lockNum=0;

static uint32 ownerArr[20]={-1};

pri16 getMaxPrio(pi_lock_t *l,pid32 p){
    pid32 hpid;
    pri16 hpri=-1;
    uint32 iterator=firstid(l->q);
	while(queuetab[iterator].qnext!=EMPTY){
        if(proctab[iterator].prprio>=hpri){
            hpri=proctab[iterator].prprio;
            hpid=iterator;
        }
	    iterator=queuetab[iterator].qnext;
	}
    if(hpri!=-1){
        uint32 lockN=l->lockNumber;
        pri16 oldprio;
        while(p!=-1 && lockN!=-1){
            p=ownerArr[lockN];
            lockN=proctab[p].waitingOn;
            oldprio=proctab[p].prprio;
            proctab[p].prprio=hpri;
            kprintf("priority_change=%d::%d-%d\n",p,oldprio,proctab[p].prprio);
        }
    }
}

syscall pi_setpark(){
    lockFlag=1;
    return OK;
}

syscall pi_park(){
    intmask	mask;			     /* Saved interrupt mask		 */
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

syscall pi_unpark(pid32 p){
    intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
    if(lockFlag==0){
        restore(mask);
        ready(p);
    }
    restore(mask);
    return OK;
}

syscall pi_initlock(pi_lock_t *l){
    l->flag=0;
    l->guard=0;
    l->q=newqueue(); 
    l->lockNumber=lockNum;
    lockNum++;
    return OK;
}

syscall pi_lock(pi_lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //sleep for 1ms to let lower prio thread reset guard
    }
    if(l->flag==0){
        l->flag=1; //acquire lock
        l->guard=0;
        //lock acquired so update owner var
        ownerArr[l->lockNumber]=currpid;
        //change currpid prio to highest prio
        getMaxPrio(l,currpid);
    }
    else{
        enqueue(currpid,l->q);
        proctab[currpid].waitingOn=l->lockNumber;
        getMaxPrio(l,ownerArr[l->lockNumber]);
        pi_setpark();
        l->guard=0;
        pi_park();
    }
    return OK;
}

syscall pi_unlock(pi_lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //acquire guard by spinning
    }
    if(isempty(l->q)){
        //lock free so update owner to -1
        ownerArr[l->lockNumber]=-1;
        proctab[currpid].waitingOn=-1;
        uint32 oldPrio=proctab[currpid].prprio;
        proctab[currpid].prprio=proctab[currpid].prevPrio;
        if(oldPrio!=proctab[currpid].prevPrio){
            kprintf("priority_change=%d::%d-%d\n",currpid,oldPrio,proctab[currpid].prevPrio);
        }
        l->flag=0; //free lock,no thread to acquire it
    }
    else{
        pid32 p=dequeue(l->q);
        uint32 old_prio=proctab[currpid].prprio;
        proctab[currpid].prprio=proctab[currpid].prevPrio;
        if(old_prio!=proctab[currpid].prevPrio){
            kprintf("priority_change=%d::%d-%d\n",currpid,old_prio,proctab[currpid].prevPrio);
        }
        ownerArr[l->lockNumber]=p;
        proctab[p].waitingOn=-1;
        getMaxPrio(l,p);
        pi_unpark(p);
    }
    l->guard=0;
    return OK;
}
