/*
 * Read from a CMOS Bank at bit-level granularity
 */
ssize_t
cmos_read(struct file *file, char *buf,
          size_t count, loff_t *ppos)
{
    struct cmos_dev *cmos_devp = file->private_data;
    char data[CMOS_BANK_SIZE];
    unsigned char mask;
    int xferred = 0, i = 0, l, zero_out;
    int start_byte = cmos_devp->current_pointer/8;
    int start_bit = cmos_devp->current_pointer%8;

    if (cmos_devp->current_pointer >= cmos_devp->size) {
        return 0; /*EOF*/
    }

    /* Adjust count if it edges past the end of the CMOS bank */
    if (cmos_devp->current_pointer + count > cmos_devp->size) {
        count = cmos_devp->size - cmos_devp->current_pointer;
    }

    /* Get the specified number of bits from the CMOS */
    while (xferred < count) {
        data[i] = port_data_in(start_byte, cmos_devp->bank_number)
            >> start_bit;
        xferred += (8 - start_bit);
        if ((start_bit) && (count + start_bit > 8)) {
            data[i] |= (port_data_in (start_byte + 1,
                        cmos_devp->bank_number) << (8 - start_bit));
            xferred += start_bit;
        }
        start_byte++;
        i++;
    }

    if (xferred > count) {
        /* Zero out (xferred-count) bits from the MSB
           of the last data byte */
        zero_out = xferred - count;
        mask = 1 << (8 - zero_out);
        for (l=0; l < zero_out; l++) {
            data[i-1] &= ~mask; mask <<= 1;
        }
        xferred = count;
    }

    if (!xferred) return -EIO;

    /* Copy the read bits to the user buffer */
    if (copy_to_user(buf, (void *)data, ((xferred/8)+1)) != 0) {
        return -EIO;
    }

    /* Increment the file pointer by the number of xferred bits */
    cmos_devp->current_pointer += xferred;

    return xferred; /* Number of bits read */
}

/*
 * Write to a CMOS bank at bit-level granularity. 'count' holds the
 * number of bits to be written.
 */
ssize_t
cmos_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
    struct cmos_dev *cmos_devp = file->private_data;
    int xferred = 0, i = 0, l, end_l, start_l;
    char *kbuf, tmp_kbuf;
    unsigned char tmp_data = 0, mask;
    int start_byte = cmos_devp->current_pointer/8;
    int start_bit = cmos_devp->current_pointer%8;

    if (cmos_devp->current_pointer >= cmos_devp->size) {
        return 0; /* EOF */
    }
    
    /* Adjust count if it edges past the end of the CMOS bank */
    if (cmos_devp->current_pointer + count > cmos_devp->size) {
        count = cmos_devp->size - cmos_devp->current_pointer;
    }

    kbuf = kmalloc((count/8)+1,GFP_KERNEL);
    if (kbuf==NULL)
        return -ENOMEM;

    /* Get the bits from the user buffer */
    if (copy_from_user(kbuf,buf,(count/8)+1)) {
        kfree(kbuf);
        return -EFAULT;
    }

    /* Write the specified number of bits to the CMOS bank */
    while (xferred < count) {
        tmp_data = port_data_in(start_byte, cmos_devp->bank_number);
        mask = 1 << start_bit;
        end_l = 8;

        if ((count-xferred) < (8 - start_bit)) {
            end_l = (count - xferred) + start_bit;
        }

        for (l = start_bit; l < end_l; l++) {
            tmp_data &= ~mask; mask <<= 1;
        }

        tmp_kbuf = kbuf[i];
        mask = 1 << end_l;
        for (l = end_l; l < 8; l++) {
            tmp_kbuf &= ~mask;
            mask <<= 1;
        }

        port_data_out(start_byte,
                      tmp_data |(tmp_kbuf << start_bit),
                      cmos_devp->bank_number);
        xferred += (end_l - start_bit);

        if ((xferred < count) && (start_bit) &&
            (count + start_bit > 8)) {
            tmp_data = port_data_in(start_byte+1,
                                    cmos_devp->bank_number);
            
            start_l = ((start_bit + count) % 8);
            mask = 1 << start_l;
            for (l=0; l < start_l; l++) {
                mask >>= 1;
                tmp_data &= ~mask;
            }
            port_data_out((start_byte+1),
                          tmp_data |(kbuf[i] >> (8 - start_bit)),
                          cmos_devp->bank_number);
            xferred += start_l;
        }
        start_byte++;
        i++;
    }

    if (!xferred) return -EIO;
    /* Push the offset pointer forward */
    cmos_devp->current_pointer += xferred;

    return xferred; /* Return the number of written bits */
}

/*
 * Read data from specified CMOS bank
 */
unsigned char
port_data_in(unsigned char offset, int bank)
{
    unsigned char data;

    if (unlikely(bank >= NUM_CMOS_BANKS)) {
        printk("Unknown CMOS Bank\n");
        return 0;
    } else {
        outb(offset, addrports[bank]); /* Read a byte */
        data = inb(dataports[bank]);
    }

    return data;
}

/*
 * Write data to specified CMOS bank
 */
void
port_data_out(unsigned char offset, unsigned char data,
              int bank)
{
    if (unlikely(bank >= NUM_CMOS_BANKS)) {
        printk("Unknown CMOS Bank\n");
        return;
    } else {
        outb(offset, addrports[bank]); /* Output a byte */
        outb(data, dataports[bank]);
    }

    return;
}