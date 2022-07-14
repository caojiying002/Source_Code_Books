
#include <linux/err.h>


char *
collect_data(char *userbuffer)
{
    char *buffer;

    /* ... */
    buffer = kmalloc(100, GFP_KERNEL);
    if (!buffer) { /* Out of memory */
        return ERR_PTR(-ENOMEM);
    }

    /* ... */

    if (copy_from_user(buffer, userbuffer, 100)) {
        return ERR_PTR(-EFAULT);
    }
    
    /* ... */
    return(buffer);
}

int
my_function(char *userbuffer)
{
    char *buf;

    /* ... */

    buf = collect_data(userbuffer);
    if (IS_ERR(buf)) {
        printk("Error returned is %d!\n", PTR_ERR(buf));
    }

    /* ... */
} 
