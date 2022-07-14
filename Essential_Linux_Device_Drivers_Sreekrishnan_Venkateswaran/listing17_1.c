
#define FLASH_START 0x00000000

#define MY_KERNEL_START 0x00080000    /* 512K for bootloader */

#define MY_FS_START 0x00280000        /* 2MB for kernel */

#define FLASH_END 0x02000000          /* 32MB */


static struct mtd_partition pda_partitions[] = {
    {
        .name = "pda_btldr",          /* This string is used by
                                         /proc/mtd to identify
                                         the bootloader partition */
        .size: = (MY_KERNEL_START-FLASH_START),
        .offset = FLASH_START,        /* Start from top of flash */
        .mask_flags = MTD_WRITEABLE   /* Read-only partition */
    },
    {
        .name = "pda_krnl",           /* Kernel partition */
        .size: = (MY_FS_START-MY_KERNEL_START),
        .offset = MTDPART_OFS_APPEND, /* Start immediately after
                                         the bootloader partition */
        .mask_flags = MTD_WRITEABLE   /* Read-only partition */
    },
    {
        .name: = "pda_fs",            /* Filesystem partition */
        .size: = MTDPART_SIZ_FULL,    /* Use up the rest of the
                                         flash */
        .offset = MTDPART_OFS_NEXTBLK,/* Align this partition with
                                         the erase size */
    }
};
