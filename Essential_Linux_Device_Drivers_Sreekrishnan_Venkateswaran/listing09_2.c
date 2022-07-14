#include <pcmcia/cistpl.h>

#include <pcmcia/ds.h>

#include <pcmcia/cs.h>

#include <pcmcia/cisreg.h>


/* This makes the XX device available to the system. XX_config()
   is based on airo_config(), defined in
   drivers/net/wireless/airo_cs.c */
static int
XX_config(struct pcmcia_device *link)
{
    tuple_t tuple;
    cisparse_t parse;
    u_char buf[64];

    /* Populate a tuple_t structure with the identity of the desired
       tuple. In this case, we're looking for a configuration table
       entry */
    tuple.DesiredTuple = CISTPL_CFTABLE_ENTRY;
    tuple.Attributes = 0;
    tuple.TupleData = buf;
    tuple.TupleDataMax = sizeof(buf);

    /* Walk the CIS for a matching tuple and glean card configuration
       information such as I/O window base addresses */
    /* Get first tuple */
    CS_CHECK(GetFirstTuple, pcmcia_get_first_tuple(link, &tuple));
    while (1){
        cistpl_cftable_entry_t dflt = {0};
        cistpl_cftable_entry_t *cfg = &(parse.cftable_entry);
        /* Read a configuration tuple from the card's CIS space */
        if (pcmcia_get_tuple_data(link, &tuple) != 0 ||
            pcmcia_parse_tuple(link, &tuple, &parse) != 0) {
            goto next_entry;
        } 
        /* We have a matching tuple! */

        /* Configure power settings in the pcmcia_device based on
           what was found in the parsed tuple entry */
        if (cfg->vpp1.present & (1<<CISTPL_POWER_VNOM))
            link->conf.Vpp = cfg->vpp1.param[CISTPL_POWER_VNOM]/10000;

        /* ... */

        /* Configure I/O window settings in the pcmcia_device based on
           what was found in the parsed tuple entry */
        if ((cfg->io.nwin > 0) || (dflt.io.nwin > 0)) {
            cistpl_io_t *io = (cfg->io.nwin) ? &cfg->io : &dflt.io;
            /* ... */
            if (!(io->flags & CISTPL_IO_8BIT)) {
                link->io.Attributes1 = IO_DATA_PATH_WIDTH_16;
            }
            link->io.BasePort1 = io->win[0].base;
            /* ... */
        }
        /* ... */
        break;
    next_entry:
        CS_CHECK(GetNextTuple, pcmcia_get_next_tuple(link, &tuple));
    }

    /* Allocate IRQ */
    if (link->conf.Attributes & CONF_ENABLE_IRQ) {
        CS_CHECK(RequestIRQ, pcmcia_request_irq(link, &link->irq));
    }

    /* ... */

    /* Invoke init_XX_card(), which is part of the generic
       XX driver (so, not shown in this Listing), and pass
       the I/O base and IRQ information obtained above */
    init_XX_card(link->irq.AssignedIRQ, link->io.BasePort1,
                 1, &handle_to_dev(link));

    /* The chip-specific (form factor independent) driver is ready
       to take responsibility of this card from now on! */
}