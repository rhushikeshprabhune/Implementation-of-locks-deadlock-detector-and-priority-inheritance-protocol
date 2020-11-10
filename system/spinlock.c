#include <xinu.h>

syscall sl_initlock(sl_lock_t *l);
syscall sl_lock(sl_lock_t *l);
syscall sl_unlock(sl_lock_t *l);
static numLocks=0;

syscall sl_initlock(sl_lock_t *l){
    if(numLocks>NLOCKS){
        return SYSERR;
    }
    l->flag=0;  // 0: lock available, 1: lock held
    numLocks++;
    return OK;
}

syscall sl_lock(sl_lock_t *l){
    while(test_and_set(&l->flag,1)==1){
        ; // spin-wait (do nothing)
    }
    return OK;
}

syscall sl_unlock(sl_lock_t *l){
    l->flag=0;
    return OK;
}




