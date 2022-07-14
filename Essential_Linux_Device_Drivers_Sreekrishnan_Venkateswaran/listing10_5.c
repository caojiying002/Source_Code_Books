/* Device-specific data structure for the Ethernet Function
   allocated during device initialization */
struct device_data {
    struct pci_dev *pdev;    /* The PCI Device structure */
    struct net_device *ndev; /* The Net Device structure */
    void *dma_buffer_rx;     /* Kernel virtual address of the
                                receive descriptor */
    dma_addr_t dma_bus_rx;   /* Bus address of the receive
                                descriptor */
    void *dma_buffer_tx;     /* Kernel virtual address of the
                                transmit descriptor */
    dma_addr_t dma_bus_tx;   /* Bus address of the transmit
                                descriptor */
    /* ... */
    spin_lock_t device_lock; /* Serialize */
} *mydev_data;

/* On-card registers related to DMA */
#define DMA_RX_REGISTER_OFFSET 0x0 /* Offset of the register

                                      holding the bus address
                                      of the RX descriptor */
#define DMA_TX_REGISTER_OFFSET 0x4 /* Offset of the register

                                      holding the bus address
                                      of the TX descriptor */
#define CONTROL_REGISTER 0x8       /* Offset of the control

                                      register */

/* Control Register Defines */
#define INITIATE_XMIT 0x1


/* Descriptor control word definitions */
#define FREE_FLAG 0x1      /* Free Descriptor */

#define INTERRUPT_FLAG 0x2 /* Assert interrupt after DMA */


/* Invoked from Listing 10.3 */
static void
dma_descriptor_setup(struct pci_dev *pdev)
{
    /* Allocate receive DMA descriptors and buffers */
    mydev_data->dma_buffer_rx =
        pci_alloc_consistent(pdev, 3096, &mydev_data->dma_bus_rx);

    /* Fill the two receive descriptors as shown in Figure 10.2 */

    /* RX descriptor 1 */
    mydev_data->dma_buffer_rx[0] = cpu_to_le32((unsigned long)
                         (mydev_data->dma_bus_rx + 24)); /* Buffer address */
    mydev_data->dma_buffer_rx[1] = 1536; /* Buffer length */
    mydev_data->dma_buffer_rx[2] = FREE_FLAG; /* Descriptor is free */

    /* RX descriptor 2 */
    mydev_data->dma_buffer_rx[3] = cpu_to_le32((unsigned long)
                         (mydev_data->dma_bus_rx + 1560)); /* Buffer address */
    mydev_data->dma_buffer_rx[4] = 1536; /* Buffer length */
    mydev_data->dma_buffer_rx[5] = FREE_FLAG; /* Descriptor is free */
    wmb(); /* Write Memory Barrier */

    /* Write the address of the receive descriptor to the appropriate
       register in the card. The I/O base address, ioaddr, was populated
       in Listing 10.3 */
    outl(cpu_to_le32((unsigned long)mydev_data->dma_bus_rx),
         ioaddr + DMA_RX_REGISTER_OFFSET);

    /* Allocate transmit DMA descriptors and buffers */
    mydev_data->dma_buffer_tx =
        pci_alloc_consistent(pdev, 3096, &mydev_data->dma_bus_tx);

    /* Fill the two transmit descriptors as shown in Figure 10.2 */

    /* TX descriptor 1 */
    mydev_data->dma_buffer_tx[0] = cpu_to_le32((unsigned long)
                         (mydev_data->dma_bus_tx + 24)); /* Buffer address */
    mydev_data->dma_buffer_tx[1] = 1536; /* Buffer length */
    /* Valid descriptor. Generate an interrupt
       after completing the DMA */
    mydev_data->dma_buffer_tx[2] = (FREE_FLAG | INTERRUPT_FLAG);

    /* TX descriptor 2 */
    mydev_data->dma_buffer_tx[3] = cpu_to_le32((unsigned long)
                         (mydev_data->dma_bus_tx + 1560)); /* Buffer address */
    mydev_data->dma_buffer_tx[4] = 1536; /* Buffer length */
    mydev_data->dma_buffer_tx[5] = (FREE_FLAG | INTERRUPT_FLAG);
    wmb(); /* Write Memory Barrier */

    /* Write the address of the transmit descriptor to the appropriate
       register in the card. The I/O base, ioaddr, was populated in
       Listing 10.3 */
    outl(cpu_to_le32((unsigned long)mydev_data->dma_bus_tx),
         ioaddr + DMA_TX_REGISTER_OFFSET);
}

/* Invoked from Listing 10.3 */
static void
dma_descriptor_release(struct pci_dev *pdev)
{
    pci_free_consistent(pdev, 3096, mydev_data->dma_bus_tx);
    pci_free_consistent(pdev, 3096, mydev_data->dma_bus_rx);
}