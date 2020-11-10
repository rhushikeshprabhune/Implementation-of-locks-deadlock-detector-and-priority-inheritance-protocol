#include <xinu.h>

syscall al_initlock(al_lock_t *l);
syscall al_lock(al_lock_t *l);
syscall al_unlock(al_lock_t *l);
bool8   al_trylock(al_lock_t *l);

syscall al_setpark();
syscall al_park();
syscall al_unpark(pid32 p);
syscall checkCycle(al_lock_t *l,pid32 p);   

static uint32 lockFlag=0;
static uint32 lockNum=0;

static uint32 ownerArr[20]={-1};

syscall checkCycle(al_lock_t *l,pid32 p){
    uint32 threadArr[100];
    uint32 i=0;
    uint32 lockN=l->lockNumber;
    while(p!=-1 && lockN!=-1){
        threadArr[i]=p;
        i++;
        p=ownerArr[lockN];
        lockN=proctab[p].waitingOn;
        if(p==threadArr[0]){
            /////////////sort array///////////////////
            uint32 k,m,a,b;
            for (k = 0; k < i; ++k){
                for (m = k + 1; m < i; ++m) {
                    if (threadArr[k] > threadArr[m]){
                        b = threadArr[k];
                        threadArr[k] = threadArr[m];
                        threadArr[m]=b;
                    }
                }
            }
            //////////////print array////////////////
            uint32 j;
            kprintf("lock_detected=");
            for(j=0;j<i;j++){
                kprintf("%d",threadArr[j]);
                if(j!=(i-1)){
                    kprintf("-");
                }
            }
            kprintf("\n");
            return SYSERR;
        }
    }
    return OK;
}

/* implementing active lock functions */
syscall al_setpark(){
    lockFlag=1;
    return OK;
}

syscall al_park(){
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

syscall al_unpark(pid32 p){
    intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();
    if(lockFlag==0){
        restore(mask);
        ready(p);
    }
    restore(mask);
    return OK;
}

syscall al_initlock(al_lock_t *l){
    l->flag=0;
    l->guard=0;
    l->q=newqueue(); 
    l->lockNumber=lockNum;
    lockNum++;
    return OK;
}

syscall al_lock(al_lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //sleep for 1ms to let lower prio thread reset guard
    }
    if(l->flag==0){
        l->flag=1; //acquire lock
        l->guard=0;
        //lock acquired so update owner var
        ownerArr[l->lockNumber]=currpid;
    }
    else{
        //thread waits on this lock so add the lock number in PCB
        proctab[currpid].waitingOn=l->lockNumber;
        //check for deadlock
        checkCycle(l,currpid);
        enqueue(currpid,l->q);
        al_setpark();
        l->guard=0;
        al_park();
    }
    return OK;
}

syscall al_unlock(al_lock_t *l){
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //acquire guard by spinning
    }
    if(isempty(l->q)){
        //lock free so update owner to -1
        ownerArr[l->lockNumber]=-1;
        proctab[currpid].waitingOn=-1;
        l->flag=0; //free lock,no thread to acquire it
    }
    else{
        pid32 p=dequeue(l->q);
        //owner changed so update the owner of the lock
        ownerArr[l->lockNumber]=p;
        proctab[p].waitingOn=-1;
        proctab[currpid].waitingOn=-1;
        al_unpark(p);
    }
    l->guard=0;
    return OK;
}

bool8 al_trylock(al_lock_t *l){
    bool8 lockAcq=1;
    while(test_and_set(&l->guard,1)==1){
        sleepms(1); //acquire guard by spinning
    }
    if(l->flag==0){
        //new owner so update the variable
        ownerArr[l->lockNumber]=currpid;
        l->flag=1; //acquire lock
        //set flag to 1 denoting thread is successful in acquiring the thread
        lockAcq=0;
    }
    l->guard=0;
    return lockAcq;
}