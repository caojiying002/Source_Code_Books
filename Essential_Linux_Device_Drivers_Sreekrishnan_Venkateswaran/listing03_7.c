static DECLARE_COMPLETION(my_thread_exit); /* Completion */
static DECLARE_WAIT_QUEUE_HEAD(my_thread_wait); /* Wait Queue */

int pink_slip = 0; /* Exit Flag */

/* Helper thread */
static int
my_thread(void *unused)
{
    DECLARE_WAITQUEUE(wait, current);
    daemonize("my_thread");
    add_wait_queue(&my_thread_wait, &wait);

    while (1) {
        /* Relinquish processor until event occurs */
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();

        /* Control gets here when the thread is woken
           up from the my_thread_wait wait queue */
        /* Quit if let go */
        if (pink_slip) {
            break;
        }

        /* Do the real work */
        /* ... */
    }

    /* Bail out of the wait queue */
    __set_current_state(TASK_RUNNING);
    remove_wait_queue(&my_thread_wait, &wait);

    /* Atomically signal completion and exit */
    complete_and_exit(&my_thread_exit, 0);
}

/* Module Initialization */
static int __init
my_init(void)
{
    /* ... */
    /* Kick start the thread */
    kernel_thread(my_thread, NULL,
                  CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
    /* ... */
}

/* Module Release */
static void __exit
my_release(void)
{
    /* ... */
    pink_slip = 1; /* my_thread must go */
    wake_up(&my_thread_wait); /* Activate my_thread */
    wait_for_completion(&my_thread_exit); /* Wait until my_thread
                                             quits */
    /* ... */
} 