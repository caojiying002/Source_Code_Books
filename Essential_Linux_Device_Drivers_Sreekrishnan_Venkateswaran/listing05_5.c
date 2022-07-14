#define CMOS_ADJUST_CHECKSUM 1

#define CMOS_VERIFY_CHECKSUM 2

#define CMOS_BANK1_CRC_OFFSET 0x1E


/*
 * Ioctls to adjust and verify CRC16s.
 */
static int
cmos_ioctl(struct inode *inode, struct file *file,
           unsigned int cmd, unsigned long arg)
{
    unsigned short crc = 0;
    unsigned char buf;

    switch (cmd) {
    case CMOS_ADJUST_CHECKSUM:
        /* Calculate the CRC of bank0 using a seed of 0 */
        crc = adjust_cmos_crc(0, 0);
        /* Seed bank1 with CRC of bank0 */
        crc = adjust_cmos_crc(1, crc);
        /* Store calculated CRC */
        port_data_out(CMOS_BANK1_CRC_OFFSET,
                      (unsigned char)(crc & 0xFF), 1);
        port_data_out((CMOS_BANK1_CRC_OFFSET + 1),
                      (unsigned char) (crc >> 8), 1);
        break;
    case CMOS_VERIFY_CHECKSUM:
        /* Calculate the CRC of bank0 using a seed of 0 */
        crc = adjust_cmos_crc(0, 0);
        /* Seed bank1 with CRC of bank0 */
        crc = adjust_cmos_crc(1, crc);
        /* Compare the calculated CRC with the stored CRC */
        buf = port_data_in(CMOS_BANK1_CRC_OFFSET, 1);
        if (buf != (unsigned char) (crc & 0xFF)) return -EINVAL;
        buf = port_data_in((CMOS_BANK1_CRC_OFFSET+1), 1);
        if (buf != (unsigned char)(crc >> 8)) return -EINVAL;
        break;
        
    default:
        return -EIO;
    }

    return 0;
}
