
#include <linux/notifier.h>

#include <asm/kdebug.h>

#include <linux/netdevice.h>

#include <linux/inetdevice.h>


/* Die Notifier Definition */
static struct notifier_block my_die_notifier = {
    .notifier_call = my_die_event_handler,
};

/* Die notification event handler */
int
my_die_event_handler(struct notifier_block *self,
                     unsigned long val, void *data)
{
    struct die_args *args = (struct die_args *)data;
    if (val == 1) { /* '1' corresponds to an "oops" */
        printk("my_die_event: OOPs! at EIP=%lx\n", args->regs->eip);
    } /* else ignore */
    return 0;
}

/* Net Device notifier definition */
static struct notifier_block my_dev_notifier = {
    .notifier_call = my_dev_event_handler,
};

/* Net Device notification event handler */
int my_dev_event_handler(struct notifier_block *self,
                         unsigned long val, void *data)
{
    printk("my_dev_event: Val=%ld, Interface=%s\n", val,
           ((struct net_device *) data)->name);
    return 0;
}

/* User-defined notifier chain implementation */
static BLOCKING_NOTIFIER_HEAD(my_noti_chain);
static struct notifier_block my_notifier = {
    .notifier_call = my_event_handler,
};

/* User-defined notification event handler */
int my_event_handler(struct notifier_block *self,
                     unsigned long val, void *data)
{
    printk("my_event: Val=%ld\n", val);
    return 0;
}

/* Driver Initialization */
static int __init
my_init(void)
{
    /* ... */
    /* Register Die Notifier */
    register_die_notifier(&my_die_notifier);

    /* Register Net Device Notifier */
    register_netdevice_notifier(&my_dev_notifier);

    /* Register a user-defined Notifier */
    blocking_notifier_chain_register(&my_noti_chain, &my_notifier);

    /* ... */
}