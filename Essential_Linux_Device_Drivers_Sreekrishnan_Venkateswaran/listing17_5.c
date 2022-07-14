#include <linux/mtd/mtd.h>

#include <stdio.h>

#include <fcntl.h>

#include <asm/ioctl.h>

#include <signal.h>

#include <sys/stat.h>


#define BLOCK_SIZE  4096

#define NUM_SECTORS 16

#define SECTOR_SIZE 64*1024


int
main(int argc, char *argv[])
{
    int fwh_fd, image_fd;
    int usect=0, lsect=0, ret;
    struct erase_info_user fwh_erase_info;
    char buffer[BLOCK_SIZE];
    struct stat statb;

    /* Ignore SIGINTR(^C) and SIGSTOP (^Z), lest
       you end up with a corrupted flash and an
       unbootable system */
    sigignore(SIGINT);
    sigignore(SIGTSTP);

    /* Open MTD char device */
    fwh_fd = open("/dev/mtd/0", O_RDWR);
    if (fwh_fd < 0) exit(1);

    /* Open BIOS image */
    image_fd = open("bios.img", O_RDONLY);
    if (image_fd < 0) exit(2);

    /* Sanity check */
    fstat(image_fd, &statb);
    if (statb.st_size != SECTOR_SIZE*NUM_SECTORS) {
        printf("BIOS image looks bad, exiting.\n");
        exit(3);
    }

    /* Unlock and erase all sectors */
    while (usect < NUM_SECTORS) {
        printf("Unlocking & Erasing Sector[%d]\r", usect+1);
        fwh_erase_info.start = usect*SECTOR_SIZE;
        fwh_erase_info.length = SECTOR_SIZE;
        ret = ioctl(fwh_fd, MEMUNLOCK, &fwh_erase_info);
        if (ret != 0) goto bios_done;
        ret = ioctl(fwh_fd, MEMERASE, &fwh_erase_info);
        if (ret != 0) goto bios_done;
        usect++;
    }

    /* Read blocks from the BIOS image and dump it to the
       Firmware Hub */
    while ((ret = read(image_fd, buffer, BLOCK_SIZE)) != 0) {
        if (ret < 0) goto bios_done;
        ret = write(fwh_fd, buffer, ret);
        if (ret <= 0) goto bios_done;
    }

    /* Verify by reading blocks from the BIOS flash and comparing
       with the image file */
    /* ... */
 bios_done:
    /* Lock back the unlocked sectors */
    while (lsect < usect) {
        printf("Relocking Sector[%d]\r", lsect+1);
        fwh_erase_info.start = lsect*SECTOR_SIZE;
        fwh_erase_info.length = SECTOR_SIZE;
        ret = ioctl(fwh_fd, MEMLOCK, &fwh_erase_info);
        if (ret != 0) printf("Relock failed on sector %d!\n", lsect);
        lsect++;
    }

    close(image_fd);
    close(fwh_fd);
} 
