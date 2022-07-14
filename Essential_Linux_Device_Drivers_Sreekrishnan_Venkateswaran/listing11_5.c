/* Read entry point */
static ssize_t
tele_read(struct file *file, char *buffer,
          size_t count, loff_t *ppos)
{
    int retval, bytes_read;
    tele_device_t *tele_device;
    /* Get the address of tele_device */
    tele_device = (tele_device_t *)file->private_data;

    /* ... */

    /* Synchronous read */
    retval = usb_bulk_msg(tele_device->usbdev, /* usb_device */
                          usb_rcvbulkpipe(tele_device->usbdev,
                                          tele_device->bulk_in_addr),
                                               /* Pipe */
                          tele_device->bulk_in_buf,
                                               /* Read buffer */
                          min(tele_device->bulk_in_len,count),
                                               /* Bytes to read */
                          &bytes_read,         /* Bytes read */
                          5000);               /* Timeout in 5 sec */

    /* Copy telemetry data to user space */
    if (!retval) {
        if (copy_to_user(buffer, tele_device->bulk_in_buf,
                         bytes_read)) {
            return -EFAULT;
        } else {
            return bytes_read;
        }
    }

    return retval;
}

/* Write entry point */
static ssize_t
tele_write(struct file *file, const char *buffer,
           size_t write_count, loff_t *ppos)
{
    char *tele_buf = NULL;
    struct urb *urb = NULL;
    tele_device_t *tele_device;

    /* Get the address of tele_device */
    tele_device = (tele_device_t *)file->private_data;

    /* ... */

    /* Allocate a bulk URB */
    urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!urb) {
        return -ENOMEM;
    }

    /* Allocate a DMA-consistent transfer buffer and copy in
       data from user space. On return, tele_buf contains
       the buffer’s CPU address, while urb->transfer_dma
       contains the DMA address */
    tele_buf = usb_buffer_alloc(tele_dev->usbdev, write_count,
                                GFP_KERNEL, &urb->transfer_dma);
    if (copy_from_user(tele_buf, buffer, write_count)) {
        usb_buffer_free(tele_device->usbdev, write_count,
                        tele_buf, urb->transfer_dma);
        usb_free_urb(urb);
        return -EFAULT
            }
    /* Populate bulk URB fields */
    usb_fill_bulk_urb(urb, tele_device->usbdev,
                      usb_sndbulkpipe(tele_device->usbdev,
                                      tele_device->bulk_out_addr),
                      tele_buf, write_count, tele_write_callback,
                      tele_device);

    /* urb->transfer_dma is valid, so preferably utilize
       that for data transfer */
    urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    /* Submit URB asynchronously */
    usb_submit_urb(urb, GFP_KERNEL);

    /* Release URB reference */
    usb_free_urb(urb);

    return(write_count);
}

/* Write callback */
static void
tele_write_callback(struct urb *urb)
{
    tele_device_t *tele_device;

    /* Get the address of tele_device */
    tele_device = (tele_device_t *)urb->context;

    /* urb->status contains the submission status. It’s 0 if
       successful. Resubmit the URB in case of errors other than
       -ENOENT, -ECONNRESET, and -ESHUTDOWN */

    /* ... */

    /* Free the transfer buffer. usb_buffer_free() is the
       release-counterpart of usb_buffer_alloc() called
       from tele_write() */
    usb_buffer_free(urb->dev, urb->transfer_buffer_length,
                    urb->transfer_buffer, urb->transfer_dma);
}
