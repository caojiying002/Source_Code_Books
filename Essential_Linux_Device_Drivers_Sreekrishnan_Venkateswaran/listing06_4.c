static void
n_touch_receive_buf(struct tty_struct *tty,
                    const unsigned char *cp, char *fp, int count)
{
    /* Work on the data in the line discipline's half of
       the flip buffer pointed to by cp */
    /* ... */

    /* Implement the Finite State Machine to interpret commands/data
       arriving from the touch controller and put the processed data
       into the local read buffer */
    /* Datasheet-dependent Code Region */
    switch (tty->disc_data->current_state) {
    case RESET:
        /* Issue a reset command to the controller */
        tty->driver->write(tty, 0, mode_stream_command,
                           sizeof(mode_stream_command));
        tty->disc_data->current_state = STREAM_DATA;
        /* ... */
        break;
    case STREAM_DATA:
        /* ... */
        break;
    case PARSING:
        /* ... */
        tty->disc_data->current_state = PARSED;
        break;
    case PARSED:
        /* ... */
    }
    if (tty->disc_data->current_state == PARSED) {
        /* If you have a parsed packet, copy the collected coordinate
           and direction information into the local read buffer */
        spin_lock_irqsave(&tty->disc_data->touch_lock, flags);
        for (i=0; i < PACKET_SIZE; i++) {
            tty->disc_data->read_buf[tty->disc_data->read_head] =
                tty->disc_data->current_pkt[i];
            tty->disc_data->read_head =
                (tty->disc_data->read_head + 1) & (BUFFER_SIZE - 1);
            tty->disc_data->read_cnt++;
        }
        spin_lock_irqrestore(&tty->disc_data->touch_lock, flags);
        /* ... */ /* See Listing 6.5 */
    }
}