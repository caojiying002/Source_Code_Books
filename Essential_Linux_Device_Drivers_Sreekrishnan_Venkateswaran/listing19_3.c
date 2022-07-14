
#include <stdio.h>

#include <linux/ioctl.h>

#include <linux/parport.h>

#include <linux/ppdev.h>

#include <fcntl.h>


int main(int argc, char *argv[])
{
    int led_fd;
    char data = 0xAA; /* Bit pattern to glow alternate LEDs */

    /* Open /dev/parport0. This assumes that the LED connector board
       is connected to the first parallel port on your computer */
    if ((led_fd = open("/dev/parport0", O_RDWR)) < 0) {
        perror("Bad Open\n");
        exit(1);
    }

    /* Claim the port */
    if (ioctl(led_fd, PPCLAIM)) {
        perror("Bad Claim\n");
        exit(2);
    }

    /* Set pins to forward direction and write a
       byte to glow alternate LEDs */
    if (ioctl(led_fd, PPWDATA, &data)) {
        perror("Bad Write\n");
        exit(3);
    }

    /* Release the port */
    if (ioctl(led_fd, PPRELEASE)) {
        perror("Bad Release\n");
        exit(4);
    }

    /* Close /dev/parport0 */
    close(led_fd);
}