/* lock.h - lock implementations */

#define NSPINLOCKS 20
#define NLOCKS     20
#define NALOCKS    20
#define NPILOCKS   20


/* struct for spinlock */
typedef struct sl_lock{
    uint32 flag;
}sl_lock_t;


/* struct for lock w/ guard */
typedef struct __lock_t{
    int flag;
    int guard;
    qid16 q;
}lock_t;


/* struct for active lock */
typedef struct al_lock{
    int flag;
    int guard;
    qid16 q;
    uint32 lockNumber;
}al_lock_t;


/* struct for pi_lock*/
typedef struct pi_lock{
    int flag;
    int guard;
    qid16 q;
    uint32 lockNumber;
}pi_lock_t;


/* spinlock functions */
syscall sl_initlock(sl_lock_t *l);
syscall sl_lock(sl_lock_t *l);
syscall sl_unlock(sl_lock_t *l);


/* lock w/ guard functions */
extern syscall initlock(lock_t *l);
extern syscall lock(lock_t *l);
extern syscall unlock(lock_t *l);


/* active lock functions */
extern syscall al_initlock(al_lock_t *l);
extern syscall al_lock(al_lock_t *l);
extern syscall al_unlock(al_lock_t *l);
extern bool8   al_trylock(al_lock_t *l);


/* pi_lock functions */
extern syscall pi_initlock(pi_lock_t *l);
extern syscall pi_lock(pi_lock_t *l);
extern syscall pi_unlock(pi_lock_t *l);