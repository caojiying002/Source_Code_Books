/* Device-specific structure */
typedef struct {
    struct usb_device *usbdev;       /* Device representation */
    struct usb_interface *interface; /* Interface representation*/
    struct urb *ctrl_urb;            /* Control URB for
                                        register access */
    struct usb_ctrlrequest ctrl_req; /* Control request
                                        as per the spec */
    unsigned char *bulk_in_buf;      /* Receive data buffer */
    size_t bulk_in_len;              /* Receive buffer size */
    __u8 bulk_in_addr;               /* IN endpoint address */
    __u8 bulk_out_addr;              /* OUT endpoint address */
    /* ... */                        /* Locks, waitqueues,
                                        statistics.. */
} tele_device_t;

#define TELE_MINOR_BASE 0xAB /* Assigned by the Linux-USB

                                subsystem maintainer */

/* Conventional char driver entry points.
   See Chapter 5, "Character Drivers." */
static struct file_operations tele_fops =
    {
        .owner = THIS_MODULE,    /* Owner */
        .read = tele_read,       /* Read method */
        .write = tele_write,     /* Write method */
        .ioctl = tele_ioctl,     /* Ioctl method */
        .open = tele_open,       /* Open method */
        .release = tele_release, /* Close method */
    };

static struct usb_class_driver tele_class = {
    .name = "tele",
    .fops = &tele_fops,            /* Connect with /dev/tele */
    .minor_base = TELE_MINOR_BASE, /* Minor number start */
};

/* The probe() method is invoked by khubd after device
   enumeration. The first argument interface, contains information
   gleaned during the enumeration process. id is the entry in the
   driver’s usb_device_id table that matches the values read from
   the telemetry card. tele_probe() is based on skel_probe()
   defined in drivers/usb/usb-skeleton.c */
static int
tele_probe(struct usb_interface *interface,
           const struct usb_device_id *id)
{
    struct usb_host_interface *iface_desc;
    struct usb_endpoint_descriptor *endpoint;
    tele_device_t *tele_device;
    int retval = -ENOMEM;

    /* Allocate the device-specific structure */
    tele_device = kzalloc(sizeof(tele_device_t), GFP_KERNEL);

    /* Fill the usb_device and usb_interface */
    tele_device->usbdev =
        usb_get_dev(interface_to_usbdev(interface));
    tele_device->interface = interface;

    /* Set up endpoint information from the data gleaned
       during device enumeration */
    iface_desc = interface->cur_altsetting;
    for (int i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (!tele_device->bulk_in_addr &&
            usb_endpoint_is_bulk_in(endpoint)) {
            /* Bulk IN endpoint */
            tele_device->bulk_in_len =
                le16_to_cpu(endpoint->wMaxPacketSize);
            tele_device->bulk_in_addr = endpoint->bEndpointAddress;
            tele_device->bulk_in_buf =
                kmalloc(tele_device->bulk_in_len, GFP_KERNEL);
        }
        if (!tele_device->bulk_out_addr &&
            usb_endpoint_is_bulk_out(endpoint)) {
            /* Bulk OUT endpoint */
            tele_device->bulk_out_addr = endpoint->bEndpointAddress;
        }
        
    }

    if (!(tele_device->bulk_in_addr && tele_device->bulk_out_addr)) {
        return retval;
    }

    /* Attach the device-specific structure to this interface.
       We will retrieve it from tele_open() */
    usb_set_intfdata(interface, tele_device);

    /* Register the device */
    retval = usb_register_dev(interface, &tele_class);
    if (retval) {
        usb_set_intfdata(interface, NULL);
        return retval;
    }

    printk("Telemetry device now attached to /dev/tele\n");
    return 0;
}

/* Disconnect method. Called when the device is unplugged or when the module is
   unloaded */
static void
tele_disconnect(struct usb_interface *interface)
{
    tele_device_t *tele_device;

    /* ... */

    /* Reverse of usb_set_intfdata() invoked from tele_probe() */
    tele_device = usb_get_intfdata(interface);

    /* Zero out interface data */
    usb_set_intfdata(interface, NULL);

    /* Release /dev/tele */
    usb_deregister_dev(interface, &tele_class);

    /* NULL the interface. In the real world, protect this
       operation using locks */
    tele_device->interface = NULL;

    /* ... */
}
