
/* The usb_driver structure */
static struct usb_driver ftdi_driver = {
    .name = "ftdi_sio",                 /* Name */
    .probe = usb_serial_probe,          /* Provided by the
                                           USB-Serial core */
    .disconnect = usb_serial_disconnect,/* Provided by the
                                           USB-Serial core */
    .id_table = id_table_combined,      /* List of supported
                                           devices built
                                           around the FTDI chip */
    .no_dynamic_id = 1,                 /* Supported ids cannot be
                                           added dynamically */
};

/* The usb_serial_driver structure */
static struct usb_serial_driver ftdi_sio_device = {
    /* ... */
    .num_ports = 1,
    .probe = ftdi_sio_probe,
    .port_probe = ftdi_sio_port_probe,
    .port_remove = ftdi_sio_port_remove,
    .open = ftdi_open,
    .close = ftdi_close,
    .throttle = ftdi_throttle,
    .unthrottle = ftdi_unthrottle,
    .write = ftdi_write,
    .write_room = ftdi_write_room,
    .chars_in_buffer = ftdi_chars_in_buffer,
    .read_bulk_callback = ftdi_read_bulk_callback,
    .write_bulk_callback = ftdi_write_bulk_callback,
    /* ... */
};

/* Driver Initialization */
static int __init ftdi_init(void)
{
    /* ... */

    /* Register with the USB-Serial core */
    retval = usb_serial_register(&ftdi_sio_device);

    /* ... */

    /* Register with the USB core */
    retval = usb_register(&ftdi_driver);

    /* ... */
}