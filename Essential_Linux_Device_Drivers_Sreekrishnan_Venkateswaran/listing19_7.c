#include <linux/i2c.h>

#include <linux/i2c-dev.h>


/* Bus addresses of the memory banks */
#define SLAVE_ADDR1 0x60

#define SLAVE_ADDR2 0x61


int
main(int argc, char *argv[])
{

    /* Open the host adapter */
    if ((smbus_fp = open("/dev/i2c-0", O_RDWR)) < 0) {
        exit(1);
    }

    /* Connect to the first bank */
    if (ioctl(smbus_fp, I2C_SLAVE, SLAVE_ADDR1) < 0) {
        exit(1);
    }

    /* ... */

    /* Dump data from the device */
    for (reg=0; reg < length; reg++) {
        /* See i2c-dev.h from the lm-sensors package for the
           implementation of the following inline function */
        res = i2c_smbus_read_byte_data(smbus_fp, (unsigned char) reg);
        if (res < 0) {
            exit(1);
        }
        /* Dump data */
        /* ... */
    }

    /* ... */

    /* Switch to bank 2 */
    if (ioctl(smbus_fp, I2C_SLAVE, SLAVE_ADDR2) < 0) {
        exit(1);
    }

    /* Clear bank 2 */
    for (reg=0; reg < length; reg+=2){
        i2c_smbus_write_word_data(smbus_fp, (unsigned char) reg, 0x0);
    }

    /* ... */

    close(smbus_fp);
} 