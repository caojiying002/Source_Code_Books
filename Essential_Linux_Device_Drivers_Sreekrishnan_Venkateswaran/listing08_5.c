int
eep_attach(struct i2c_adapter *adapter, int address, int kind)
{
    static struct i2c_client *eep_client;

    eep_client = kmalloc(sizeof(*eep_client), GFP_KERNEL);
    eep_client->driver = &eep_driver; /* Registered in Listing 8.2 */
    eep_client->addr = address;       /* Detected Address */
    eep_client->adapter = adapter;    /* Host Adapter */
    eep_client->flags = 0;
    strlcpy(eep_client->name, "eep", I2C_NAME_SIZE);

    /* Populate fields in the associated per-device data structure */
    /* ... */

    /* Attach */
    i2c_attach_client(new_client);
}
