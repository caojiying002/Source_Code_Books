
static struct platform_driver pda_map_driver = {
    .driver = {
        .name = "pda",         /* ID */
    },
    .probe    = pda_mtd_probe, /* Probe */
    .remove   = NULL,          /* Release */
    .suspend  = NULL,          /* Power management */
    .resume   = NULL,          /* Power management */
};

/* Driver/module Initialization */
static int __init pda_mtd_init(void)
{
    return platform_driver_register(&pda_map_driver);
}

/* Module Exit */
static int __init pda_mtd_exit(void)
{
    return platform_driver_uregister(&pda_map_driver);
}

