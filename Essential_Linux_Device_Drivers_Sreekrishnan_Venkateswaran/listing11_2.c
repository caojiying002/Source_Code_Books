#define USB_TELE_VENDOR_ID 0xABCD /* Manufacturer’s Vendor ID */

#define USB_TELE_PRODUCT_ID 0xCDEF /* Device’s Product ID */


/* USB ID Table specifying the devices that this driver supports */
static struct usb_device_id tele_ids[] = {
    { USB_DEVICE(USB_TELE_VENDOR_ID, USB_TELE_PRODUCT_ID) },
    { } /* Terminate */
};

MODULE_DEVICE_TABLE(usb, tele_ids);

/* The usb_driver structure for this driver */
static struct usb_driver tele_driver =
    {
        .name = "tele",                /* Unique name */
        .probe = tele_probe,           /* See Listing 11.3 */
        .disconnect = tele_disconnect, /* See Listing 11.3 */
        .id_table = tele_ids,          /* See above */
    };

/* Module Initialization */
static int __init
usb_tele_init(void)
{
    /* Register with the USB core */
    result = usb_register(&tele_driver);

    /* ... */

    return 0;
}

/* Module Exit */
static void __exit
usb_tele_exit(void)
{
    /* Unregister from the USB core */
    usb_deregister(&tele_driver);
    return;
}

module_init(usb_tele_init);
module_exit(usb_tele_exit);
