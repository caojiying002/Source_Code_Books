/* Based on drivers/edac/i82860_edac.c */
#define I855_PCI_DEVICE_ID   0x3584 /* PCI Device ID of the memory

                                       controller in the 855 GME */
#define I855_ERRSTS_REGISTER 0x62   /* Error Status Register's offset

                                       in the PCI configuration space */
#define I855_EAP_REGISTER    0x98   /* Error Address Pointer Register's

                                       offset in the PCI configuration space */

struct i855_error_info {
    u16 errsts; /* Error Type */
    u32 eap; /* Error Location */
};

/* Get error information */
static void
i855_get_error_info(struct mem_ctl_info *mci,
                    struct i855_error_info *info)
{
    struct pci_dev *pdev;
 
    pdev = to_pci_dev(mci->dev);

    /* Read error type */
    pci_read_config_word(pdev, I855_ERRSTS_REGISTER, &info->errsts);

    /* Read error location */
    pci_read_config_dword(pdev, I855_EAP_REGISTER, &info->eap);
}

/* Process errors */
static int
i855_process_error_info(struct mem_ctl_info *mci,
                        struct i855_error_info *info,
                        int handle_errors)
{
    int row;

    info->eap >>= PAGE_SHIFT;
    row = edac_mc_find_csrow_by_page(mci, info->eap); /* Find culprit row */

    /* Handle using services provided by the EDAC core.
       Populate sysfs, generate error messages, and so on */
    
    if (is_MBE()) {        /* is_MBE() looks at I855_ERRSTS_REGISTER and checks
                              for an MBE. Implementation not shown */
        edac_mc_handle_ue(mci, info->eap, 0, row, "i855 UE");
    } else if (is_SBE()) { /* is_SBE() looks at I855_ERRSTS_REGISTER and checks
                              for an SBE. Implementation not shown */
        edac_mc_handle_ce(mci, info->eap, 0, info->derrsyn, row, 0,
                          "i855 CE");
    }

    return 1;
}

/* This method is registered with the EDAC core from i855_probe() */
static void
i855_check(struct mem_ctl_info *mci)
{
    struct i855_error_info info;

    i855_get_error_info(mci, &info);
    i855_process_error_info(mci, &info, 1);
}

/* The PCI driver probe method, part of the pci_driver structure */
static int
i855_probe(struct pci_dev *pdev, int dev_idx)
{
    struct mem_ctl_info *mci;

    /* ... */

    pci_enable_device(pdev);

    /* Allocate control memory for this memory controller.
       The 3 arguments to edac_mc_alloc() correspond to the
       amount of requested private storage, number of chip-select
       rows, and number of channels in your memory layout */
    mci = edac_mc_alloc(0, CSROWS, CHANNELS);

    /* ... */

    mci->edac_check = i855_check; /* Supply the check method to the
                                     EDAC core */
    /* Do other memory controller initializations */

    /* ... */

    /* Register this memory controller with the EDAC core */
    edac_mc_add_mc(mci, 0);
    /* ... */
}

/* Remove method */
static void __devexit
i855_remove(struct pci_dev *pdev)
{
    struct mem_ctl_info *mci = edac_mc_find_mci_by_pdev(pdev);

    if (mci && !edac_mc_del_mc(mci)) {
        edac_mc_free(mci); /* Free memory for this controller. Reverse
                              of edac_mc_alloc() */
    }
}

/* PCI Device ID Table */
static const struct pci_device_id i855_pci_tbl[] __devinitdata = {
    {PCI_VEND_DEV(INTEL, I855_PCI_DEVICE_ID),
     PCI_ANY_ID, PCI_ANY_ID, 0, 0,},
    {0,},
};

MODULE_DEVICE_TABLE(pci, i855_pci_tbl);
/* pci_driver structure for this device.
   Re-visit Chapter 10 for a detailed explanation */
static struct pci_driver i855_driver = {
    .name     = "855",
    .probe    = i855_probe,
    .remove   = __devexit_p(i855_remove),
    .id_table = i855_pci_tbl,
};

/* Driver Initialization */
static int __init
i855_init(void)
{
    /* ... */
    
    pci_rc = pci_register_driver(&i855_driver);

    /* ... */
}