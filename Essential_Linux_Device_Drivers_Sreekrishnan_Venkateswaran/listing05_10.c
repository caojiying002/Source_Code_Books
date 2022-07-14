#include <fcntl.h>

#include <asm/types.h>

#include <linux/watchdog.h>


int
main()
{
    int new_timeout;

    int wfd = open("/dev/watchdog", O_WRONLY);

    /* Set the watchdog timeout to 20 seconds */
    new_timeout = 20;
    ioctl(fd, WDIOC_SETTIMEOUT, &new_timeout);

    while (1) {
        /* Graphics processing */
        process_graphics();
        /* Pet the watchdog */
        ioctl(fd, WDIOC_KEEPALIVE, 0);
        /* Or instead do: write(wfd, "\0", 1); */
        fsync(wfd);
    }
}