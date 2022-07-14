#include <linux/delay.h>

#include <net/irda/irda.h>

#include "sir-dev.h" /* Assume that this sample driver lives in

                        drivers/net/irda/ */

/* Open Method. This is invoked when an irattach is issued on the
   associated UART */
static int
mydongle_open(struct sir_dev *dev)
{
    struct qos_info *qos = &dev->qos;

    /* Power the dongle by setting modem control lines, DTR/RTS. */
    sirdev_set_dtr_rts(dev, TRUE, TRUE);

    /* Speeds that mydongle can accept */
    qos->baud_rate.bits &= IR_19200|IR_57600;
    irda_qos_bits_to_value(qos); /* Set QoS */

    return 0;
}

/* Change baud rate */
static int
mydongle_change_speed(struct sir_dev *dev, unsigned speed)
{
    if ((speed == 19200) || (speed = 57600)){
        /* Toggle the speed by pulsing RTS low
           for 50 us and back high for 25 us */
        sirdev_set_dtr_rts(dev, TRUE, FALSE);
        udelay(50);
        sirdev_set_dtr_rts(dev, TRUE, TRUE);
        udelay(25);
        return 0;
    } else {
        return -EINVAL;
    }
}

/* Reset */
static int
mydongle_reset(struct sir_dev *dev)
{
    /* Reset the dongle as per the spec, for example,
       by pulling DTR low for 50 us */
    sirdev_set_dtr_rts(dev, FALSE, TRUE);
    udelay(50);
    sirdev_set_dtr_rts(dev, TRUE, TRUE);
    dev->speed = 19200; /* Reset speed is 19200 baud */
    return 0;
}

/* Close */
static int
mydongle_close(struct sir_dev *dev)
{
    /* Power off the dongle as per the spec,
       for example, by pulling DTR and RTS low.. */
    sirdev_set_dtr_rts(dev, FALSE, FALSE);

    return 0;
}

/* Dongle Driver Methods */
static struct dongle_driver mydongle = {
    .owner = THIS_MODULE,
    .type = MY_DONGLE,                  /* Add this to the enumeration
                                           in include/linux/irda.h */
    .open = mydongle_open,              /* Open */
    .reset = mydongle_reset,            /* Reset */
    .set_speed = mydongle_change_speed, /* Change Speed */
    .close = mydongle_close,            /* Close */
};

/* Initialize */
static int __init
mydongle_init(void)
{
    /* Register the entry points */
    return irda_register_dongle(&mydongle);
}

/* Release */
static void __exit
mydongle_cleanup(void)
{
    /* Unregister entry points */
    irda_unregister_dongle(&mydongle);
}

module_init(mydongle_init);
module_exit(mydongle_cleanup);