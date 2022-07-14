static DECLARE_WAIT_QUEUE_HEAD(myevent_waitqueue);

rwlock_t myevent_lock;
extern unsigned int myevent_id; /* Holds the identity of the
                                   troubled data structure.
                                   Populated later on */

static int mykthread(void *unused)
{
    unsigned int event_id = 0;
    DECLARE_WAITQUEUE(wait, current);

    /* Become a kernel thread without attached user resources */
    daemonize("mykthread");

    /* Request delivery of SIGKILL */
    allow_signal(SIGKILL);

    /* The thread sleeps on this wait queue until it's
       woken up by parts of the kernel in charge of sensing
       the health of data structures of interest */
    add_wait_queue(&myevent_waitqueue, &wait);

    for (;;) {
        /* Relinquish the processor until the event occurs */
        set_current_state(TASK_INTERRUPTIBLE);
        schedule(); /* Allow other parts of the kernel to run */
        /* Die if I receive SIGKILL */
        if (signal_pending(current)) break;
        
        /* Control gets here when the thread is woken up */
        read_lock(&myevent_lock); /* Critical section starts */
        if (myevent_id) { /* Guard against spurious wakeups */
            event_id = myevent_id;
            read_unlock(&myevent_lock); /* Critical section ends */
            /* Invoke the registered user mode helper and
               pass the identity code in its environment */
            run_umode_handler(event_id); /* Expanded later on */
        } else {
            read_unlock(&myevent_lock);
        }
    }

    set_current_state(TASK_RUNNING);
    remove_wait_queue(&myevent_waitqueue, &wait);
    return 0;
}