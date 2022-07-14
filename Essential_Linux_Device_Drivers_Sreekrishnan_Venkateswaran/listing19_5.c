#include <stdio.h>

#include <fcntl.h>

#include <sys/ioctl.h>

#include <scsi/sg.h>


#define RCAP_COMMAND 0x25

#define RCAP_COMMAND_LEN 10

#define RCAP_REPLY_LEN 8


int
main(int argc, char *argv[])
{
    int fd, i;
    /* READ_CAPACITY command block */
    unsigned char RCAP_CmdBlk[RCAP_COMMAND_LEN]=
        {RCAP_COMMAND, 0,0,0,0,0,0,0,0,0};
    sg_io_hdr_t sg_io;
    unsigned char rcap_buff[RCAP_REPLY_LEN];
    unsigned int lastblock, blocksize;
    unsigned long long disk_cap;
    unsigned char sense_buf[32];

    /* Open the sg device */
    if ((fd = open("/dev/sg0", O_RDONLY)) < 0) {
        printf("Bad Open\n");
        exit(1);
    }

    /* Initialize */
    memset(&sg_io, 0, sizeof(sg_io_hdr_t));

    /* Command block address and length */
    sg_io.cmdp = RCAP_CmdBlk;
    sg_io.cmd_len = RCAP_COMMAND_LEN;

    /* Response buffer address and length */
    sg_io.dxferp = rcap_buff;
    sg_io.dxfer_len = RCAP_REPLY_LEN;

    /* Sense buffer address and length */
    sg_io.sbp = sense_buf;
    sg_io.mx_sb_len = sizeof(sense_buf);
    
    /* Control information */
    sg_io.interface_id = 'S';
    sg_io.dxfer_direction = SG_DXFER_FROM_DEV;
    sg_io.timeout = 10000; /* 10 seconds */

    /* Issue the SG_IO ioctl */
    if (ioctl(fd, SG_IO, &sg_io) < 0) {
        printf("Bad SG_IO\n");
        exit(1);
    }

    /* Obtain results */
    if ((sg_io.info & SG_INFO_OK_MASK) == SG_INFO_OK) {
        /* Address of last disk block */
        lastblock = ((rcap_buff[0]<<24)|(rcap_buff[1]<<16)|
                     (rcap_buff[2]<<8)|(rcap_buff[3]));
        /* Block size */
        blocksize = ((rcap_buff[4]<<24)|(rcap_buff[5]<<16)|
                     (rcap_buff[6]<<8)|(rcap_buff[7]));
        /* Calculate disk capacity */
        disk_cap = (lastblock+1);
        disk_cap *= blocksize;
        printf("Disk Capacity = %llu Bytes\n", disk_cap);
    }

    close(fd);
}