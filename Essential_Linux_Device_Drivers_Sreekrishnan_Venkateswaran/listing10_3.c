#include <linux/pci.h>


unsigned long ioaddr;

/* Probe method */
static int __devinit
net_driver_probe(struct pci_dev *pdev,
                 const struct pci_device_id *id)
{

    /* The net_device structure is defined in include/linux/netdevice.h.
       See Chapter 15, "Network Interface Cards," for the description */
    struct net_device *net_dev;

    /* Ask low-level PCI code to enable I/O and memory regions for
       this device. Look up the IRQ for the device that the PCI
       subsystem allotted when it walked the bus */
    pci_enable_device(pdev);

    /* Use this device in bus mastering mode, since the network
       function of this card is capable of DMA */
    pci_set_master(pdev);

    /* Allocate an Ethernet interface and fill in generic values in
       the net_dev structure. prv_data is the private driver data
       structure that contains buffers, locks, and so on. This is
       left undefined. Wait until Chapter 15 for more on
       alloc_etherdev() */
    net_dev = alloc_etherdev(sizeof(struct prv_data));

    /* Populate net_dev with your network device driver methods */
    net_dev->hard_start_xmit = &mydevice_xmit; /* See Listing 10.6 */

    /* More net_dev initializations */
    /* ... */

    /* Get the I/O address for this PCI region. All card registers
       specified in Table 10.3 are assumed to be in bar 0 */
    ioaddr = pci_resource_start(pdev, 0);

    /* Claim a 128-byte I/O region */
    request_region(ioaddr, 128, "ntwrk");

    /* Fill in resource information obtained from the PCI layer */
    net_dev->base_addr = ioaddr;
    net_dev->irq = pdev->irq;

    /* ... */

    /* Setup DMA. Defined in Listing 10.5 */
    dma_descriptor_setup(pdev);

    /* Register the driver with the network layer. This will allot
       an unused ethX interface */
    register_netdev(net_dev);

    /* ... */
}

/* Remove method */
static void __devexit
net_driver_remove(struct pci_dev *pdev)
{
    /* Free transmit and receive DMA buffers.
       Defined in Listing 10.5 */
    dma_descriptor_release(pdev);

    /* Release memory regions */
    /* ... */

    /* Unregister from the networking layer */
    unregister_netdev(dev);
    free_netdev(dev);

    /* ... */
} 