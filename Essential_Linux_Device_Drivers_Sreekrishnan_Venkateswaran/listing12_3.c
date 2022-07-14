
#include <linux/console.h>

#include <linux/serial_core.h>

#include <asm/io.h>


#define USB_UART_PORTS 2 /* The cell phone has 2

                            USB_UART ports */

/* Each USB_UART has a 3-byte register set consisting of
   UU_STATUS_REGISTER at offset 0, UU_READ_DATA_REGISTER at
   offset 1, and UU_WRITE_DATA_REGISTER at offset 2, as shown
   in Table One of Chapter 6, "Serial Drivers" */
#define USB_UART1_BASE          0xe8000000 /* Memory base for USB_UART1 */

#define USB_UART2_BASE          0xe9000000 /* Memory base for USB_UART1 */

#define USB_UART_REGISTER_SPACE 0x3


/* Semantics of bits in the status register */
#define USB_UART_TX_FULL        0x20

#define USB_UART_RX_EMPTY       0x10

#define USB_UART_STATUS         0x0F

#define USB_UART1_IRQ           3

#define USB_UART2_IRQ           4

#define USB_UART_CLK_FREQ       16000000

#define USB_UART_FIFO_SIZE      32


/* Parameters of each supported USB_UART port */
static struct uart_port usb_uart_port[] = {
    {
        .mapbase = (unsigned int)USB_UART1_BASE,
        .iotype = UPIO_MEM,             /* Memory mapped */
        .irq = USB_UART1_IRQ,           /* IRQ */
        .uartclk = USB_UART_CLK_FREQ,   /* Clock HZ */
        .fifosize = USB_UART_FIFO_SIZE, /* Size of the FIFO */
        .flags = UPF_BOOT_AUTOCONF,     /* UART port flag */
        .line = 0,                      /* UART Line number */
    },
    {
        .mapbase = (unsigned int)USB_UART2_BASE,
        .iotype = UPIO_MEM,             /* Memory mapped */
        .irq = USB_UART2_IRQ,           /* IRQ */
        .uartclk = USB_UART_CLK_FREQ,   /* CLock HZ */
        .fifosize = USB_UART_FIFO_SIZE, /* Size of the FIFO */
        .flags = UPF_BOOT_AUTOCONF,     /* UART port flag */
        .line = 1,                      /* UART Line number */
    }
};

/* Write a character to the USB_UART port */
static void
usb_uart_putc(struct uart_port *port, unsigned char c)
{
    /* Wait until there is space in the TX FIFO of the USB_UART.
       Sense this by looking at the USB_UART_TX_FULL
       bit in the status register */
    while (__raw_readb(port->membase) & USB_UART_TX_FULL);
    /* Write the character to the data port*/
    __raw_writeb(c, (port->membase+1));
}

/* Console write */
static void
usb_uart_console_write(struct console *co, const char *s,
                       u_int count)
{
    int i;
    /* Write each character */
    for (i = 0; i < count; i++, s++) {
        usb_uart_putc(&usb_uart_port[co->index], *s);
    }
}

/* Get communication parameters */
static void __init
usb_uart_console_get_options(struct uart_port *port,
                             int *baud, int *parity, int *bits)
{
    /* Read the current settings (possibly set by a bootloader)
       or return default values for parity, number of data bits,
       and baud rate */
    *parity = 'n';
    *bits = 8;
    *baud = 115200;
}

/* Setup console communication parameters */
static int __init
usb_uart_console_setup(struct console *co, char *options)
{
    struct uart_port *port;
    int baud, bits, parity, flow;

    /* Validate port number and get a handle to the
       appropriate structure */
    if (co->index == -1 || co->index >= USB_UART_PORTS) {
        co->index = 0;
    }
    port = &usb_uart_port[co->index];

    /* Use functions offered by the serial layer to parse options */
    if (options) {
        uart_parse_options(options, &baud, &parity, &bits, &flow);
    } else {
        usb_uart_console_get_options(port, &baud, &parity, &bits);
    }

    return uart_set_options(port, co, baud, parity, bits, flow);
}

/* Populate the console structure */
static struct console usb_uart_console = {
    .name = "ttyUU", /* Console name */
    .write = usb_uart_console_write, /* How to printk to the
                                        console */
    .device = uart_console_device, /* Provided by the serial core */
    .setup = usb_uart_console_setup, /* How to setup the console */
    .flags = CON_PRINTBUFFER, /* Default flag */
    .index = -1, /* Init to invalid value */
};

/* Console Initialization */
static int __init
usb_uart_console_init(void)
{
    /* ... */

    /* Register this console */
    register_console(&usb_uart_console);

    return 0;
}

console_initcall(usb_uart_console_init); /* Mark console init */
