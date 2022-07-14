/* Offset of the Telemetry configuration register
   within the on-card register space */
#define TELEMETRY_CONFIG_REG_OFFSET 0x0A


/* Value to program in the configuration register */
#define TELEMETRY_CONFIG_REG_VALUE 0xBC


/* The vendor-defined bRequest for programming the
   configuration register */
#define TELEMETRY_REQUEST_WRITE 0x0D


/* The vendor-defined bRequestType */
#define TELEMETRY_REQUEST_WRITE_REGISTER 0x0E


/* Open method */
static int
tele_open(struct inode *inode, struct file *file)
{
    struct completion tele_config_done;
    tele_device_t *tele_device;
    void *tele_ctrl_context;
    char *tmp;
    __le16 tele_config_index = TELEMETRY_CONFIG_REG_OFFSET;    
    unsigned int tele_ctrl_pipe;
    struct usb_interface *interface;

    /* Obtain the pointer to the device-specific structure.
       We saved it using usb_set_intfdata() in tele_probe() */
    interface = usb_find_interface(&tele_driver, iminor(inode));
    tele_device = usb_get_intfdata(interface);

    /* Allocate a URB for the control transfer */
    tele_device->ctrl_urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!tele_device->ctrl_urb) {
        return -EIO;
    }

    /* Populate the Control Request */
    tele_device->ctrl_req.bRequestType = TELEMETRY_REQUEST_WRITE;
    tele_device->ctrl_req.bRequest =
        TELEMETRY_REQUEST_WRITE_REGISTER;
    tele_device->ctrl_req.wValue =
        cpu_to_le16(TELEMETRY_CONFIG_REG_VALUE);
    tele_device->ctrl_req.wIndex =
        cpu_to_le16p(&tele_config_index);
    tele_device->ctrl_req.wLength = cpu_to_le16(1);
    tele_device->ctrl_urb->transfer_buffer_length = 1;
    tmp = kmalloc(1, GFP_KERNEL);
    *tmp = TELEMETRY_CONFIG_REG_VALUE;

    /* Create a control pipe attached to endpoint 0 */
    tele_ctrl_pipe = usb_sndctrlpipe(tele_device->usbdev, 0);

    /* Initialize the completion mechanism */
    init_completion(&tele_config_done);

    /* Set URB context. The context is part of the URB that is passed
       to the callback function as an argument. In this case, the
       context is the completion structure, tele_config_done */
    tele_ctrl_context = (void *)&tele_config_done;

    /* Initialize the fields in the control URB */
    usb_fill_control_urb(tele_device->ctrl_urb, tele_device->usbdev,
                         tele_ctrl_pipe,
                         (char *) &tele_device->ctrl_req,
                         tmp, 1, tele_ctrl_callback,
                         tele_ctrl_context);
    

    /* Submit the URB */
    usb_submit_urb(tele_device->ctrl_urb, GFP_ATOMIC);

    /* Wait until the callback returns indicating that the telemetry
       configuration register has been successfully initialized */
    wait_for_completion(&tele_config_done);

    /* Release our reference to the URB */
    usb_free_urb(urb);
    kfree(tmp);

    /* Save the device-specific object to the file’s private_data
       so that you can directly retrieve it from other entry points
       such as tele_read() and tele_write() */
    file->private_data = tele_device;

    /* Return the URB transfer status */
    return(tele_device->ctrl_urb->status);
}

/* Callback function */
static void
tele_ctrl_callback(struct urb *urb)
{
    complete((struct completion *)urb->context);
}
