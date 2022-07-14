    /* n_touch_receive_buf() continued.. */
    /* Wake up any threads waiting for data */
    if (waitqueue_active(&tty->read_wait) &&
            (tty->read_cnt >= tty->minimum_to_wake))
        wake_up_interruptible(&tty->read_wait);
    }

    /* If we are running out of buffer space, request the
       serial driver to throttle incoming data */
    if (n_touch_receive_room(tty) < TOUCH_THROTTLE_THRESHOLD) {
        tty->driver.throttle(tty);
    }
    /* ... */