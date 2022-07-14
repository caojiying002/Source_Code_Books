#include <pcmcia/ds.h> /* Definition of struct pcmcia_device */


static struct pcmcia_driver XX_cs_driver = {
    .owner = THIS_MODULE,
    .drv = {
        .name = "XX_cs",   /* Name */
    },
    .probe = XX_probe,     /* Probe */
    .remove = XX_remove,   /* Release */
    .id_table = XX_ids,    /* ID table */
    .suspend = XX_suspend, /* Power management */
    .resume = XX_resume,   /* Power management */
};

#define XX_MANFUFACTURER_ID 0xABCD /* Device's manf_id */

#define XX_CARD_ID 0xCDEF          /* Device's card_id */


/* Identity of supported cards */
static struct pcmcia_device_id XX_ids[] = {
    PCMCIA_DEVICE_MANF_CARD(XX_MANFUFACTURER_ID, XX_CARD_ID),
    PCMCIA_DEVICE_NULL,
};

MODULE_DEVICE_TABLE(pcmcia, XX_ids); /* For module autoload */

/* Initialization */
static int __init
init_XX_cs(void)
{
    return pcmcia_register_driver(&XX_cs_driver);
}

/* Probe Method */
static int
XX_probe(struct pcmcia_device *link)
{
    /* Populate the pcmcia_device structure allotted for this card by
       the core. First fill in general information */
    /* ... */
 
   /* Fill in attributes related to I/O windows and
       interrupt levels */
    XX_config(link); /* See Listing 9.2 */
}