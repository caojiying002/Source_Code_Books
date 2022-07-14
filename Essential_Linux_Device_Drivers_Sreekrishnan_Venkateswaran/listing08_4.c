#include <linux/i2c.h>


/* The EEPROM has two memory banks having addresses SLAVE_ADDR1
 * and SLAVE_ADDR2, respectively
 */
static unsigned short normal_i2c[] = {
    SLAVE_ADDR1, SLAVE_ADDR2, I2C_CLIENT_END
};

static struct i2c_client_address_data addr_data = {
    .normal_i2c = normal_i2c,
    .probe = ignore,
    .ignore = ignore,
    .forces = ignore,
};

static int
eep_probe(struct i2c_adapter *adapter)
{
    /* The callback function eep_attach(), is shown
     * in Listing 8.5
     */
    return i2c_probe(adapter, &addr_data, eep_attach);
}