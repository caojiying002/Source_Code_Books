int
eep_open(struct inode *inode, struct file *file)
{
    /* The EEPROM bank to be opened */
    n = MINOR(file->f_dentry->d_inode->i_rdev);
    file->private_data = (struct ee_bank *)ee_bank_list[n];

    /* Initialize the fields in ee_bank_list[n] such as
       size, slave address, and the current file pointer */

    /* ... */
}