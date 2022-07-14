static int
mydrv_worker(void *unused)
{
    DECLARE_WAITQUEUE(wait, current);
    void (*worker_func)(void *);
    void *worker_data;
    struct _mydrv_work *mydrv_work;

    set_current_state(TASK_INTERRUPTIBLE);

    /* Spin until asked to die */
    while (!asked_to_die()) {
        add_wait_queue(&mydrv_wq.todo, &wait);

        if (list_empty(&mydrv_wq.mydrv_worklist)) {
            schedule();
            /* Woken up by the submitter */
        } else {
            set_current_state(TASK_RUNNING);
        }

        remove_wait_queue(&mydrv_wq.todo, &wait);

        /* Protect concurrent access to the list */
        spin_lock(&mydrv_wq.lock);

        /* Traverse the list and plough through the work functions
           present in each node */

        while (!list_empty(&mydrv_wq.mydrv_worklist)) {
            /* Get the first entry in the list */
            mydrv_work = list_entry(mydrv_wq.mydrv_worklist.next,
                                    struct _mydrv_work, mydrv_workitem);
            worker_func = mydrv_work->worker_func;
            worker_data = mydrv_work->worker_data;
            /* This node has been processed. Throw it
               out of the list */
            list_del(mydrv_wq.mydrv_worklist.next);
            kfree(mydrv_work); /* Free the node */
            /* Execute the work function in this node */
            spin_unlock(&mydrv_wq.lock); /* Release lock */
            worker_func(worker_data);
            spin_lock(&mydrv_wq.lock); /* Re-acquire lock */
        }

        spin_unlock(&mydrv_wq.lock);
        set_current_state(TASK_INTERRUPTIBLE);
    }

    set_current_state(TASK_RUNNING);
    return 0;
} 