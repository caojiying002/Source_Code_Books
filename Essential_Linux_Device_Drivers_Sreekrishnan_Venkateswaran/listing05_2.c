
/*
 * Open CMOS bank
 */
int
cmos_open(struct inode *inode, struct file *file)
{
    struct cmos_dev *cmos_devp;

    /* Get the per-device structure that contains this cdev */
    cmos_devp = container_of(inode->i_cdev, struct cmos_dev, cdev);

    /* Easy access to cmos_devp from rest of the entry points */
    file->private_data = cmos_devp;

    /* Initialize some fields */
    cmos_devp->size = CMOS_BANK_SIZE;
    cmos_devp->current_pointer = 0;

    return 0;
}

/*
 * Release CMOS bank
 */
int
cmos_release(struct inode *inode, struct file *file
{
    struct cmos_dev *cmos_devp = file->private_data;

    /* Reset file pointer */
    cmos_devp->current_pointer = 0;

    return 0;
}