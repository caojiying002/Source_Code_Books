#include <linux/trace.h>


int data_packet, driver_data; /* Trace events */

/* Driver init */
static int __init mydriver_init(void)
{
    /* ... */

    /* Event to dump packets received from the device */
    data_packet = trace_create_event("data_pkt",
                                     NULL,
                                     CUSTOM_EVENT_FORMAT_TYPE_HEX,
                                     NULL);
    /* Event to dump a driver structure */
    driver_data = trace_create_event("dvr_data",
                                     NULL,
                                     CUSTOM_EVENT_FORMAT_TYPE_HEX,
                                     NULL);

    /* ... */
}
