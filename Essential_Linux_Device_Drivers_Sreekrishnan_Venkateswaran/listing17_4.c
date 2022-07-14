
    /* Snippet from cfi_probe_chip() (2.6.23.1 kernel) defined in
       drivers/mtd/chips/cfi_probe.c, with comments added */

    /* cfi is a pointer to struct cfi_private defined in
       include/linux/mtd/cfi.h */

    /* ... */

    /* Ask the device to enter query mode by sending
       0x98 to offset 0x55 */
    cfi_send_gen_cmd(0x98, 0x55, base, map, cfi,
                     cfi->device_type, NULL);

    /* If the device did not return the ASCII characters
       'Q’, 'R’ and 'Y’, the chip is not CFI-compliant */
    if (!qry_present(map, base, cfi)) {
        xip_enable(base, map, cfi);
        return 0;
    }

    /* Elicit chip parameters and the command-set, and populate
       the cfi structure */
    if (!cfi->numchips) {
        return cfi_chip_setup(map, cfi);
    }

    /* ... */
