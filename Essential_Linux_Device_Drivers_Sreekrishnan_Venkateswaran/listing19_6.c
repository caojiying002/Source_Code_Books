#include <usb.h>                  /* From the libusb package */


#define DIGICAM_VENDOR_ID 0x04b0  /* From /proc/bus/usb/devices */

#define DIGICAM_PRODUCT_ID 0x0205 /* From /proc/bus/usb/devices */


int
main(int argc, char *argv[])
{
    struct usb_dev_handle *mydevice_handle;
    struct usb_bus *usb_bus;
    struct usb_device *mydevice;
    
    /* Initialize libusb */
    usb_init();
    usb_find_buses();
    usb_find_devices();

    /* Walk the bus */
    for (usb_bus = usb_buses; usb_bus; usb_bus = usb_bus->next) {
        for (mydevice = usb_bus->devices; mydevice;
             mydevice = mydevice->next) {
            if ((mydevice->descriptor.idVendor == DIGICAM_VENDOR_ID) &&
                (mydevice->descriptor.idProduct == DIGICAM_PRODUCT_ID)) {

                /* Open the device */
                mydevice_handle = usb_open(mydevice);

                /* Send commands to the camera. This is the heart of the
                   driver. Getting information about the USB control
                   messages to which your device responds is often a
                   challenge since many vendors do not readily divulge
                   hardware details */
                usb_control_msg(mydevice_handle, ...);

                /* ... */

                /* Close the device */
                usb_close(mydevice_handle);
            }
        }
    }
}