spinlock_t roller_lock = SPIN_LOCK_UNLOCKED;
static DECLARE_WAIT_QUEUE_HEAD(roller_poll);

static irqreturn_t
roller_interrupt(int irq, void *dev_id)
{
    int i, PA_t, PA_delta_t, movement = 0;

    /* Get the waveforms from bits 0, 1 and 2
       of Port D as shown in Figure 4.3 */
    PA_t = PA_delta_t = PORTD & 0x07;

    /* Wait until the state of the pins change.
       (Add some timeout to the loop) */
    for (i=0; (PA_t==PA_delta_t); i++){
        PA_delta_t = PORTD & 0x07;
    }

    movement = determine_movement(PA_t, PA_delta_t); /* See below */

    spin_lock(&roller_lock);
    /* Store the wheel movement in a buffer for
       later access by the read()/poll() entry points */
    store_movements(movement);
    spin_unlock(&roller_lock);

    /* Wake up the poll entry point that might have
       gone to sleep, waiting for a wheel movement */
    wake_up_interruptible(&roller_poll);

    return IRQ_HANDLED;
}

int
determine_movement(int PA_t, int PA_delta_t)
{

    switch (PA_t){
    case 0:
        switch (PA_delta_t){
        case 1:
            movement = ANTICLOCKWISE;
            break;
        case 2:
            movement = CLOCKWISE;
            break;
        case 4:
            movement = KEYPRESSED;
            break;
        }
        break;
    case 1:
        switch (PA_delta_t){
        case 3:
            movement = ANTICLOCKWISE;
            break;
        case 0:
            movement = CLOCKWISE;
            break;
        }
        break;
    case 2:
        switch (PA_delta_t){
        case 0:
            movement = ANTICLOCKWISE;
            break;
        case 3:
            movement = CLOCKWISE;
            break;
        }
        break;
    case 3:
        switch (PA_delta_t){
        case 2:
            movement = ANTICLOCKWISE;
            break;
        case 1:
            movement = CLOCKWISE;
            break;
        }
    case 4:
        movement = KEYPRESSED;
        break;
    }
} 