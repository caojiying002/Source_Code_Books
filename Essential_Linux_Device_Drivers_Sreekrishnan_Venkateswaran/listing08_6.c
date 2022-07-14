
ssize_t
eep_read (struct file *file, char *buf,
          size_t count, loff_t *ppos)
{
    int i, transferred, ret, my_buf[BANK_SIZE];

    /* Get the private client data structure for this bank */
    struct ee_bank *my_bank =
        (struct ee_bank *)file->private_data;

    /* Check whether the smbus_read_word() functionality is
       supported */
    if (i2c_check_functionality(my_bank->client,
                                I2C_FUNC_SMBUS_READ_WORD_DATA)) {
        /* Read the data */
        while (transferred < count) {
            ret = i2c_smbus_read_word_data(my_bank->client,
                                           my_bank->current_pointer+i);
            my_buf[i++] = (u8)(ret & 0xFF);
            my_buf[i++] = (u8)(ret >> 8);
            transferred += 2;
        }

        /* Copy data to user space and increment the internal
           file pointer. Sanity checks are omitted for simplicity */
        copy_to_user(buffer, (void *)my_buf, transferred);
        my_bank->current_pointer += transferred;
    }

    return transferred;
}