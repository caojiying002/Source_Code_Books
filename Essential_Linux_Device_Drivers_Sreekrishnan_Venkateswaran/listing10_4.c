
/* Probe method */
static int __devinit
modem_driver_probe(struct pci_dev *pdev,
                   const struct pci_device_id *id)
{
    struct uart_port port; /* See Chapter 6, "Serial Drivers" */

    /* Ask low-level PCI code to enable I/O and memory regions
       for this PCI device */
    pci_enable_device(pdev);

    /* Get the PCI IRQ and I/O address to be used and populate the
       uart_port structure (see Chapter 6) with these resources. Look at
       include/linux/pci.h for helper functions */
    /* ... */

    /* Register this information with the serial layer and get an
       unused ttySX port allotted to the card. Look at Chapter 6 for
       more on serial drivers */
    serial8250_register_port(&port);

    /* ... */
}

/* Remove method */
static void __devexit
modem_driver_remove(struct pci_dev *dev)
{
    /* Unregister the port from the serial layer */
    serial8250_unregister_port(&port);

    /* Disable device */
    pci_disable_device(dev);
} 