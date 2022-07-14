
/*
 * Seek to a bit offset within a CMOS bank
 */
static loff_t
cmos_llseek(struct file *file, loff_t offset,
            int orig)
{
    struct cmos_dev *cmos_devp = file->private_data;

    switch (orig) {
    case 0: /* SEEK_SET */
        if (offset >= cmos_devp->size) {
            return -EINVAL;
        }
        cmos_devp->current_pointer = offset; /* Bit Offset */
        break;
    case 1: /* SEEK_CURR */
        if ((cmos_devp->current_pointer + offset) >=
            cmos_devp->size) {
            return -EINVAL;
        }
        cmos_devp->current_pointer = offset; /* Bit Offset */
        break;
    case 2: /* SEEK_END - Not supported */
        return -EINVAL;
    default:
        return -EINVAL;
    }

    return(cmos_devp->current_pointer);
}