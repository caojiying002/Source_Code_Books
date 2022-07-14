#include <linux/pci.h>


#define MY_VENDOR_ID     0xABCD

#define MY_DEVICE_ID_NET 0xEF01


/* The set of PCI cards that this driver supports. Only a single
   entry in our case. Look at include/linux/mod_devicetable.h for
   the definition of pci_device_id */
struct pci_device_id network_driver_pci_table[] __devinitdata = 
    {
        { MY_VENDOR_ID,               /* Interface chip manufacturer ID */
          MY_DEVICE_ID_NET,           /* Device ID for the network
                                         function */
          PCI_ANY_ID,                 /* Subvendor ID wild card */
          PCI_ANY_ID,                 /* Subdevice ID wild card */
          0, 0,                       /* class and classmask are
                                         unspecified */
          network_driver_private_data /* Use this to co-relate
                                         configuration information if the
                                         driver supports multiple
                                         cards. Can be an enumerated type. */
        }, {0},
    };

/* struct pci_driver is defined in include/linux/pci.h */
struct pci_driver network_pci_driver = {
    .name = "ntwrk",                         /* Unique name */
    .probe = net_driver_probe,               /* See Listing 10.3 */
    .remove = __devexit_p(net_driver_remove),/* See Listing 10.3 */
    .id_table = network_driver_pci_table,    /* See above */
    /* suspend() and resume() methods that implement power
       management are not used by this driver */
};

/* Ethernet driver initialization */
static int __init
network_driver_init(void)
{
    pci_register_driver(&network_pci_driver);
    return 0;
}

/* Ethernet driver exit */
static void __exit
network_driver_exit(void)
{
    pci_unregister_driver(&network_pci_driver);
}

module_init(network_driver_init);
module_exit(network_driver_exit);

MODULE_DEVICE_TABLE(pci, network_driver_pci_table);
