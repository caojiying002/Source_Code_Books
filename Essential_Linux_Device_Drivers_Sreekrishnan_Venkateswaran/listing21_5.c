#include <linux/kernel.h>

#include <linux/module.h>

#include <linux/kprobes.h>

#include <linux/kallsyms.h>


/* kretprobe at exit from tty_open() */
static int
kret_tty_open(struct kretprobe_instance *kreti,
              struct pt_regs *regs)
{
    /* The EAX register contains the function return value
       on x86 systems */
    if ((int) regs->eax) {
        /* tty_open() failed. Announce the return code */
        printk("tty_open returned %d\n", (int)regs->eax);
    }

    return 0;
}

/* Per-probe structure */
static struct kretprobe kretprobe_eg = {
    .handler = (kretprobe_handler_t)kret_tty_open
};

int
init_module(void)
{
    int retval;

    kretprobe_eg.kp.addr = (kprobe_opcode_t*)
        kallsyms_lookup_name("tty_open");
    if (!kretprobe_eg.kp.addr) {
        printk("Bad Probe Point\n");
        return -1;
    }

    /* Register the kretprobe */
    if ((retval = register_kretprobe(&kretprobe_eg) < 0)) {
        printk("register_kretprobe error, return value=%d\n",
               retval);
        return -1;
    }

    printk("kretprobe registered.\n");
    return 0;
}

void module_cleanup(void)
{
    unregister_kretprobe(&kretprobe_eg);
}

MODULE_LICENSE("GPL");