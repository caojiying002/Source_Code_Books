/* Private structure used to implement the Finite State Machine
   (FSM) for the touch controller. The controller and the processor
   communicate using a specific protocol that the FSM implements */
struct n_touch {
    int current_state;      /* Finite State Machine */
    spinlock_t touch_lock;  /* Spinlock */
    struct tty_struct *tty; /* Associated tty */
    /* Statistics and other housekeeping */
    /* ... */
} *n_tch;

/* Device open() */
static int
n_touch_open(struct tty_struct *tty)
{
    /* Allocate memory for n_tch */
    if (!(n_tch = kmalloc(sizeof(struct n_touch), GFP_KERNEL))) {
        return -ENOMEM;
    }
    memset(n_tch, 0, sizeof(struct n_touch));
    tty->disc_data = n_tch; /* Other entry points now
                               have direct access to n_tch */

    /* Allocate the line discipline's local read buffer
       used for copying data out of the tty flip buffer */
    tty->read_buf = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!tty->read_buf) return -ENOMEM;

    /* Clear the read buffer */
    memset(tty->read_buf, 0, BUFFER_SIZE);

    /* Initialize lock */
    spin_lock_init(&ntch->touch_lock);

    /* Initialize other necessary tty fields.
       See drivers/char/n_tty.c for an example */

    /* ... */

    return 0;
} 
