#include <linux/kernel.h>

#include <linux/module.h>

#include <linux/kprobes.h>

#include <linux/kallsyms.h>


/* Jprobe the entrance to printk */
asmlinkage int
jprintk(const char *fmt, ...)
{
    for (; *fmt; ++fmt) {
        if ((*fmt=='%')&&(*(fmt+1) == 'O')) *(char *)(fmt+1) = 'o';
    }
    jprobe_return();
    return 0;
}

/* Per-probe structure */
static struct jprobe jprobe_eg = {
    .entry = (kprobe_opcode_t *) jprintk
};

int
init_module(void)
{
    int retval;

    jprobe_eg.kp.addr = (kprobe_opcode_t*)
        kallsyms_lookup_name("printk");
    if (!jprobe_eg.kp.addr) {
        printk("Bad probe point\n");
        return -1;
    }

    /* Register the Jprobe */
    if ((retval = register_jprobe(&jprobe_eg) < 0)) {
        printk("register_jprobe error, return value=%d\n",
               retval);
        return -1;
    }

    printk("Jprobe registered.\n");
    return 0;
}

void
module_cleanup(void)
{
    unregister_jprobe(&jprobe_eg);
}

MODULE_LICENSE("GPL");
