#include <linux/netdevice.h>

#include <linux/etherdevice.h>

#include <linux/skbuff.h>

#include <linux/ethtool.h>


struct net_device_stats mycard_stats; /* Statistics */

/* Fill ethtool_ops methods from a suitable place in the driver */
struct ethtool_ops mycard_ethtool_ops = {
    /* ... */
    .get_eeprom = mycard_get_eeprom, /* Dump EEPROM contents */
    /* ... */
};

/* Initialize/probe the card. For PCI cards, this is invoked
   from (or is itself) the probe() method. In that case, the
   function is declared as:
   static struct net_device *init_mycard(struct pci_dev *pdev, const
   struct pci_device_id *id)
*/
static struct net_device *
init_mycard()
{
    struct net_device *netdev;
    struct priv_struct mycard_priv;

    /* ... */
    netdev = alloc_etherdev(sizeof(struct priv_struct));

    /* Common methods */
    netdev->open = &mycard_open;
    netdev->stop = &mycard_close;
    netdev->do_ioctl = &mycard_ioctl;

    /* Data transfer */
    netdev->hard_start_xmit = &mycard_xmit_frame; /* Transmit */
    netdev->poll = &mycard_poll;                  /* Receive - NAPI */
    netdev->weight = 64;                          /* Fairness */

    /* Watchdog */
    netdev->tx_timeout = &mycard_timeout; /* Recovery function */
    netdev->watchdog_timeo = 8*HZ;        /* 8-second timeout */

    /* Statistics and configuration */
    netdev->get_stats = &mycard_get_stats;     /* Statistics support */
    netdev->ethtool_ops = &mycard_ethtool_ops; /* Ethtool support */
    netdev->set_mac_address = &mycard_set_mac; /* Change MAC */
    netdev->change_mtu = &mycard_change_mtu;   /* Alter MTU */
    strncpy(netdev->name, pci_name(pdev),
            sizeof(netdev->name) - 1);         /* Name (for PCI) */

    /* Bus-specific parameters. For a PCI NIC, it looks as follows */
    netdev->mem_start = pci_resource_start(pdev, 0);
    netdev->mem_end = netdev->mem_start + pci_resource_len(pdev, 0);

    /* Register the interface */
    register_netdev(netdev);

    /* ... */

    /* Get MAC address from attached EEPROM */
    /* ... */

    /* Download microcode if needed */
    /* ... */
}

/* The interrupt handler */
static irqreturn_t
mycard_interrupt(int irq, void *dev_id)
{
    struct net_device *netdev = dev_id;
    struct sk_buff *skb;
    unsigned int length;

    /* ... */

    if (receive_interrupt) {
        /* We were interrupted due to packet reception. At this point,
           the NIC has already DMA'ed received data to an sk_buff that
           was pre-allocated and mapped during device open. Obtain the
           address of the sk_buff depending on your data structure
           design and assign it to 'skb'. 'length' is similarly obtained
           from the NIC by reading the descriptor used to DMA data from
           the card. Now, skb->data contains the received data. */
        /* ... */

        /* For PCI cards, perform a pci_unmap_single() on the
           received buffer in order to allow the CPU to access it */
        /* ... */

        /* Allow the data go to the tail of the packet by moving
           skb->tail down by length bytes and increasing
           skb->len correspondingly */
        skb_put(skb, length)
            /* Pass the packet to the TCP/IP stack */
#if !defined (USE_NAPI) /* Do it the old way */

            netif_rx(skb);
#else /* Do it the NAPI way */

        if (netif_rx_schedule_prep(netdev)) {
            /* Disable NIC interrupt. Implementation not shown. */
            disable_nic_interrupt();
            /* Post the packet to the protocol layer and
               add the device to the poll list */
            __netif_rx_schedule(netdev);
        }
#endif

    } else if (tx_complete_interrupt) {
        /* Transmit Complete Interrupt */
        /* ... */
        /* Unmap and free transmit resources such as
           DMA descriptors and buffers. Free sk_buffs or
           reclaim them into a free pool */
        /* ... */
    }
}

/* Driver open */
static int
mycard_open(struct net_device *netdev)
{
    /* ... */

    /* Request irq */
    request_irq(irq, mycard_interrupt, IRQF_SHARED,
                netdev->name, dev);

    /* Allocate Descriptor rings */
    /* See the section,
       "Buffer Management and Concurrency Control" */
    /* ... */

    /* Provide free descriptor addresses to the card */
    /* ... */

    /* Convey your readiness to accept data from the
       networking stack */
    netif_start_queue(netdev);

    /* ... */
}

/* Driver close */
static int
mycard_close(struct net_device *netdev)
{
    /* ... */

    /* Ask the networking stack to stop sending down data */
    netif_stop_queue(netdev);

    /* ... */
}

/* Called when the device is unplugged or when the module is
   released. For PCI cards, this is invoked from (or is itself)
   the remove() method. In that case, the function is declared as:
   static void __devexit mycard_remove(struct pci_dev *pdev)
*/
static void __devexit
mycard_remove()
{
    struct net_device *netdev;

    /* ... */

    /* For a PCI card, obtain the associated netdev as follows,
       assuming that the probe() method performed a corresponding
       pci_set_drvdata(pdev, netdev) after allocating the netdev */
    netdev = pci_get_drvdata(pdev); 
    unregister_netdev(netdev); /* Reverse of register_netdev() */
    /* ... */
    free_netdev(netdev);       /* Reverse of alloc_netdev() */
    /* ... */
}

/* Suspend method. For PCI devices, this is part of
   the pci_driver structure discussed in Chapter 10 */
static int
mycard_suspend(struct pci_dev *pdev, pm_message_t state)
{
    /* ... */
    netif_device_detach(netdev);
    /* ... */
}

/* Resume method. For PCI devices, this is part of
   the pci_driver structure discussed in Chapter 10 */
static int
mycard_resume(struct pci_dev *pdev)
{
    /* ... */
    netif_device_attach(netdev);
    /* ... */
}

/* Get statistics */
static struct net_device_stats *
mycard_get_stats(struct net_device *netdev)
{
    /* House keeping */
    /* ... */
    return(&mycard_stats);
}

/* Dump EEPROM contents. This is an ethtool_ops operation */
static int
mycard_get_eeprom(struct net_device *netdev,
                  struct ethtool_eeprom *eeprom, uint8_t *bytes)
{
    /* Read data from the accompanying EEPROM */
    /* ... */
}

/* Poll method */
static int
mycard_poll(struct net_device *netdev, int *budget)
{
    /* Post packets to the protocol layer using
       netif_receive_skb() */
    /* ... */
    if (no_more_ingress_packets()){
        /* Remove the device from the polled list */
        netif_rx_complete(netdev);

        /* Fall back to interrupt mode. Implementation not shown */
        enable_nic_interrupt();
        return 0;
    }
}

/* Transmit method */
static int
mycard_xmit_frame(struct sk_buff *skb, struct net_device *netdev)
{
    /* DMA the transmit packet from the associated sk_buff
       to card memory */
    /* ... */
    /* Manage buffers */
    /* ... */
}
