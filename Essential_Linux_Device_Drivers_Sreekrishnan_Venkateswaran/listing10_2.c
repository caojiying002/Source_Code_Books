#include <linux/pci.h>


#define MY_VENDOR_ID     0xABCD

#define MY_DEVICE_ID_MDM 0xEF02


/* The set of PCI cards that this driver supports */
struct pci_device_id modem_driver_pci_table[] __devinitdata =
    {
        { MY_VENDOR_ID,             /* Interface chip manufacturer ID */
          MY_DEVICE_ID_MDM,         /* Device ID for the modem
                                       function */
          PCI_ANY_ID,               /* Subvendor ID wild card */
          PCI_ANY_ID,               /* Subdevice ID wild card */
          0, 0,                     /* class and classmask are
                                       unspecified */
          modem_driver_private_data /* Use this to co-relate
                                       configuration information if the driver
                                       supports multiple cards. Can be an
                                       enumerated type. */
        }, {0},
    };

struct pci_driver modem_pci_driver = {
    .name = "mdm",                             /* Unique name */
    .probe = modem_driver_probe,               /* See Listing 10.4 */
    .remove = __devexit_p(modem_driver_remove),/* See Listing 10.4 */
    .id_table = modem_driver_pci_table,        /* See above */
    /* suspend() and resume() methods that implement power
       management are not used by this driver */
};

/* Modem driver initialization */
static int __init
modem_driver_init(void)
{
    pci_register_driver(&modem_pci_driver);
    return 0;
}

/* Modem driver exit */
static void __exit
modem_driver_exit(void)
{
    pci_unregister_driver(&modem_pci_driver);
}

module_init(modem_driver_init);
module_exit(modem_driver_exit);

MODULE_DEVICE_TABLE(pci, modem_driver_pci_table);