static int __init
mydrv_init(void)
{
    /* Initialize the lock to protect against
       concurrent list access */
    spin_lock_init(&mydrv_wq.lock);

    /* Initialize the wait queue for communication
       between the submitter and the worker */
    init_waitqueue_head(&mydrv_wq.todo);

    /* Initialize the list head */
    INIT_LIST_HEAD(&mydrv_wq.mydrv_worklist);

    /* Start the worker thread. See Listing 3.4 */
    kernel_thread(mydrv_worker, NULL,
                  CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);

    return 0;
}