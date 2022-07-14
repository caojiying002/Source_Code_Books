int npages=0;
EXPORT_SYMBOL(npages);

static int memwalkd(void *unused)
{
    long curr_pfn = (64*1024*1024 >> PAGE_SHIFT);
    struct page *curr_page;

    /* ... */

    daemonize("memwalkd"); /* kernel thread */
    sigfillset(&current->blocked);
    allow_signal(SIGUSR1);

    for (;;) {
        /* Dequeue a signal if it's pending */
        if (signal_pending(current)) {
            sig = dequeue_signal(current, &current->blocked, &info);

            /* Point A */

            /* Free npages pages when SIGUSR1 is received */
            if (sig == SIGUSR1) {

                /* Point B */

                /* Problem manifests when npages crosses 10 in the following
                   loop. Let’s apply run time medication here via Kprobes */
                for (i=0; i < npages; i++, curr_pfn++) {
                    /* ... */
                }
            }
            /* ... */
        }
        /* ... */
    }

}