struct mydriver_data driver_data; /* Private device structure */

/* Driver read() method */
ssize_t
mydriver_read(struct file *file, char *buf,
              size_t count, loff_t *ppos)
{
    char *buffer;

    /* Read numbytes bytes of data from the device into
       buffer */
    /* ... */

    /* Dump data Packet. If you see the problem only
       under certain conditions, say, when the packet length is
       greater than a value, use that as a filter */
    if (condition) {
        /* See Listing 21.7 for the definition of data_packet*/
        trace_raw_event(data_packet, numbytes, buffer);
    }

    /* Dump driver data structures */
    if (some_other_condition) {
        /* See Listing 21.7 for the definition of driver_data */
        trace_raw_event(driver_data, sizeof(driver_data), &driver_data);
    }

    /* ... */
}
