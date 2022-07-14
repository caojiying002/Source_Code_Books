#include <linux/ioport.h>


void
dump_port( unsigned char addr_port, unsigned char data_port,
           unsigned short offset, unsigned short length)
{
    unsigned char i, *data;

    if (!(data = (unsigned char *)malloc(length))) {
        perror("Bad Malloc\n");
        exit(1);
    }

    /* Write the offset to the index port
       and read data from the data port */
    for(i=offset; i<offset+length; i++) {
        outb(i, addr_port );
        data[i-offset] = inb(data_port);
    }

    /* Dump */
    for(i=0; i<length; i++)
        printf("%02X ", data[i]);

    free(data);
}

int
main(int argc, char *argv[])
{
    /* Get access permissions */
    if( iopl(3) < 0) {
        perror("iopl access error\n");
        exit(1);
    }
    dump_port(0x70, 0x71, 0x0, 1);
}