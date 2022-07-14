#include <linux/miscdevice.h>

#include <linux/watchdog.h>


#define DEFAULT_WATCHDOG_TIMEOUT 10 /* 10-second timeout */

#define TIMEOUT_SHIFT 5             /* To get to the timeout field

                                       in WD_CONTROL_REGISTER */
#define WENABLE_SHIFT 3             /* To get to the

                                       watchdog-enable field in
                                       WD_CONTROL_REGISTER */

/* Misc structure */
static struct miscdevice my_wdt_dev = {
    .minor = WATCHDOG_MINOR, /* defined as 130 in
                                include/linux/miscdevice.h */
    .name = "watchdog",      /* /dev/watchdog */
    .fops = &my_wdt_dog      /* Watchdog driver entry points */
};

/* Driver methods */
struct file_operations my_wdt_dog = {
    .owner = THIS_MODULE,
    .open = my_wdt_open,
    .release = my_wdt_close,
    .write = my_wdt_write,
    .ioctl = my_wdt_ioctl
}

/* Module Initialization */
static int __init
my_wdt_init(void)
{
    /* ... */
    misc_register(&my_wdt_dev);
    /* ... */
}

/* Open watchdog */
static void
my_wdt_open(struct inode *inode, struct file *file)
{
    /* Set the timeout and enable the watchdog */
    WD_CONTROL_REGISTER |= DEFAULT_WATCHDOG_TIMEOUT << TIMEOUT_SHIFT;
    WD_CONTROL_REGISTER |= 1 << WENABLE_SHIFT;
}

/* Close watchdog */
static int
my_wdt_close(struct inode *inode, struct file *file)
{
    /* If CONFIG_WATCHDOG_NOWAYOUT is chosen during kernel
       configuration, do not disable the watchdog even if the
       application desires to close it */
#ifndef CONFIG_WATCHDOG_NOWAYOUT

    /* Disable watchdog */
    WD_CONTROL_REGISTER &= ~(1 << WENABLE_SHIFT);
#endif

    return 0;
}

/* Pet the dog */
static ssize_t
my_wdt_write(struct file *file, const char *data,
             size_t len, loff_t *ppose)
{
    /* Pet the dog by writing a specified sequence of bytes to the
       watchdog service register */
    WD_SERVICE_REGISTER = 0xABCD;
}

/* Ioctl method. Look at Documentation/watchdog/watchdog-api
   for the full list of ioctl commands. This is standard across
   watchdog drivers, so conforming applications are rendered
   hardware-independent */
static int
my_wdt_ioctl(struct inode *inode, struct file *file,
             unsigned int cmd, unsigned long arg)
{
    /* ... */
    switch (cmd) {
    case WDIOC_KEEPALIVE:
        /* Write to the watchdog. Applications can invoke
           this ioctl instead of writing to the device */
        WD_SERVICE_REGISTER = 0xABCD;
        break;
    case WDIOC_SETTIMEOUT:
        copy_from_user(&timeout, (int *)arg, sizeof(int));
        /* Set the timeout that defines unresponsiveness by
           writing to the watchdog control register */
        WD_CONTROL_REGISTER = timeout << TIMEOUT_BITS;
        break;
    case WDIOC_GETTIMEOUT:
        /* Get the currently set timeout from the watchdog */
        /* ... */
        break;
    default:
        return -ENOTTY;
    }
}

/* Module Exit */
static void __exit
my_wdt_exit(void)
{
    /* ... */
    misc_deregister(&my_wdt_dev);
    /* ... */
}

module_init(my_wdt_init);
module_exit(my_wdt_exit);