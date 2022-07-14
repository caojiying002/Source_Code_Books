/* Driver entry points */
static struct file_operations eep_fops = {
    .owner = THIS_MODULE,
    .llseek = eep_llseek,
    .read = eep_read,
    .ioctl = eep_ioctl,
    .open = eep_open,
    .release = eep_release,
    .write = eep_write,
};

static dev_t dev_number;            /* Allotted Device Number */
static struct class *eep_class;     /* Device class */
/* Per-device client data structure for each
 * memory bank supported by the driver
 */
struct eep_bank {
    struct i2c_client *client;      /* I2C client for this bank */
    unsigned int addr;              /* Slave address of this bank */
    unsigned short current_pointer; /* File pointer */
    int bank_number;                /* Actual memory bank number */
    /* ... */                       /* Spinlocks, data cache for
                                       slow devices,.. */
};

#define NUM_BANKS 2                 /* Two supported banks */

#define BANK_SIZE 2048              /* Size of each bank */

struct ee_bank *ee_bank_list;       /* List of private data
                                       structures, one per bank */

/*
 * Device Initialization
 */
int __init
eep_init(void)
{
    int err, i;

    /* Allocate the per-device data structure, ee_bank */
    ee_bank_list = kmalloc(sizeof(struct ee_bank)*NUM_BANKS,
                           GFP_KERNEL);
    memset(ee_bank_list, 0, sizeof(struct ee_bank)*NUM_BANKS);

    /* Register and create the /dev interfaces to access the EEPROM
       banks. Refer back to Chapter 5, "Character Drivers" for
       more details */
    if (alloc_chrdev_region(&dev_number, 0,
                            NUM_BANKS, "eep") < 0) {
        printk(KERN_DEBUG "Can’t register device\n");
        return -1;
    }

    eep_class = class_create(THIS_MODULE, DEVICE_NAME);

    for (i=0; i < NUM_BANKS;i++) {
        /* Connect the file operations with cdev */
        cdev_init(&ee_bank[i].cdev, &ee_fops);
        /* Connect the major/minor number to the cdev */
        if (cdev_add(&ee_bank[i].cdev, (dev_number + i), 1)) {
            printk("Bad kmalloc\n");
            return 1;
        }
        device_create(eep_class, NULL, MKDEV(MAJOR(dev_number), i),
                      "eeprom%d", i);
    }

    /* Inform the I2C core about our existence. See the section
       "Probing the Device" for the definition of eep_driver */
    err = i2c_add_driver(&eep_driver);
    if (err) {
        printk("Registering I2C driver failed, errno is %d\n", err);
        return err;
    }

    printk("EEPROM Driver Initialized.\n");
    return 0;
}