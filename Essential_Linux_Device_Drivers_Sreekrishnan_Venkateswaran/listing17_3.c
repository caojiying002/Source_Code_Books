#include <linux/mtd/mtd.h>

#include <linux/mtd/map.h>

#include <linux/ioport.h>


static int
pda_mtd_probe(struct platform_device *pdev)
{
    struct map_info *pda_map;
    struct mtd_info *pda_mtd;
    struct resource *res = pdev->resource;

    /* Populate pda_map with information obtained
       from the associated platform device */
    pda_map->virt = ioremap_nocache(res->start,
                                    (res->end - res->start + 1));
    pda_map->name = pdev->dev.bus_id;
    pda_map->phys = res->start;
    pda_map->size = res->end - res->start + 1;
    pda_map->bankwidth = 2;    /* Two 16-bit banks sitting
                                  on a 32-bit bus */
    simple_map_init(&pda_map); /* Fill in default access methods */

    /* Probe via the CFI chip driver */
    pda_mtd = do_map_probe("cfi_probe", &pda_map);

    /* Register the mtd_partition structure */
    add_mtd_partitions(pda_mtd, pda_partitions, 3); /* Three Partitions */

    /* ... */
}